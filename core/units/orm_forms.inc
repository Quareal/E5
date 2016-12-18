<?php

function insert_values($table_id,$module_id,$arow=0,$force=0,$prefix='',$flush_posts=1,$col_source=0,$ignore_defs=0,$required_fill_cols=0){
	global $db,$cum;
	$id=$module_id;
	$sid=$arow;
	//if(empty($table_id))	$cols=getall($db,"SELECT * FROM main_col WHERE col_module=$module_id AND col_table=$table_id AND col_inform=1 ORDER BY col_pos, col_bold",1,'main_col',__LINE__,__FILE__);
	//else 		$cols=getall($db,"SELECT * FROM main_col WHERE col_table=$table_id AND (col_inform=1 OR col_oninsert!='') ORDER BY col_pos, col_bold",1,'main_col',__LINE__,__FILE__);
	//Убираем проверку по col_inform в связи с добавлением определения выпавших столбцов
	if(empty($table_id) || $col_source==1){
			$search_table=$table_id;
			if($col_source==1) $search_table=0;
			$cols=getall($db,"SELECT * FROM main_col WHERE col_module=$module_id AND col_table=$search_table ORDER BY col_pos, col_bold",1,'main_col',__LINE__,__FILE__);
			//foreach($cols AS $c) echo $c['col_sname'].'<br>';
	} else	{
		$cols=load_cols_by_table($table_id);//getall($db,"SELECT * FROM main_col WHERE col_table=$table_id ORDER BY col_pos, col_bold",1,'main_col',__LINE__,__FILE__);	
	}
	//echo '<br>';

	//определение выпавших столбцов
	if(!$ignore_defs && isset($GLOBALS["f_action"]) && $GLOBALS['f_action']=='edit' && isset($GLOBALS['backrow'])) foreach($cols AS $col){	
		if(!isset($_POST[$prefix."col".$col["col_id"]]) && !isset($_FILES[$prefix."col".$col["col_id"]]) && empty($_POST[$prefix."col".$col["col_id"].'_check']) && empty($_POST[$prefix."col".$col["col_id"].'_serv'])){
			$row=$GLOBALS['backrow'];
			$c=$col["col_id"];
			$xx=getall($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=$c AND value_module=$module_id AND value_table=$table_id",1,"row_value");
			if(!empty($xx)){
				//$pre_exists[$col["col_id"]]=1; //если разместить тут, то приводит к удалению значений-файлов, которые скрыты из формы изменения
				if($col["col_type"]==3){
					$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
							VALUES ($id, $table_id, $sid, $c, '".safe_sql_input($xx[0]["value_value"],1)."')",3,'row_value',__LINE__,__FILE__);
				} else {
					$pre_exists[$col["col_id"]]=1;
					if($col["col_link2"]==0 || ($col["col_type"]!=1 && $col["col_type"]!=5)){
						//echo $prefix."col".$col["col_id"].' - '.$xx[0]["value_value"].'<br>';
						$_POST[$prefix."col".$col["col_id"]]=prepend_value($xx[0]["value_value"]);
					} else foreach($xx AS $x){
						$_POST[$prefix."col".$col["col_id"]][$x["value_value"]]=1;
					}
				}
			}
		}
	}
	
	//перебор столбцов
	foreach($cols AS $col){
		foreach($col AS $var=>$value) $$var=$value;
		//echo $col_sname.'<br>';
		
		if($ignore_defs) $col_default='';		
		//выходим если поле не заполнено и обработчик запустился из переопределения настроек для подтаблиц
		if(empty($_POST[$prefix."col".$col_id]) && empty($_FILES[$prefix."col".$col_id]) && $required_fill_cols) continue;
		
		if($col_required && empty($_POST[$prefix."col".$col_id]) && empty($_FILES[$prefix."col".$col_id])){
			$GLOBALS["cancel"]='Не заполнено обязательное поле «'.$col_name.'»';
			break;
		}
		if(isset($pre_exists[$col_id])) $db->query("DELETE FROM row_value WHERE value_module=$id AND value_table=$table_id AND value_row=$sid AND value_col=$col_id",3,"row_value",__LINE__,__FILE__);
		if($col_type==0){
			if(isset($_POST[$prefix."col".$col_id])){
				$val=$_POST[$prefix."col".$col_id];
				//if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]!=-1) $val=utf8_win($_POST[$prefix."col".$col_id]); //это точно надо править 
			} else $val=$col_default;
			//if($col_id==120) var_dump($val);
			//if($col_id==25) echo $val.'!!!';
			//$val=prepend_value($val);
			//if(!empty($_POST[$prefix."col".$col_id])) $val=str_replace("'","''",$val);	этот обработчик вроде как работает ещё раньше на посте
			//if(!empty($_POST[$prefix."col".$col_id])) $val=str_replace("\\","\\\\",$val);	//? это под вопросом]
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
					VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($val,1)."')",3,'row_value',__LINE__,__FILE__);
			if($col_tpl) update_part_links($sid,'val',$col_uin,$val,'row',$table_id,$id);
		}
		if($col_type==6){			
			if(isset($_POST[$prefix."col".$col_id])){
				$val=$_POST[$prefix."col".$col_id];
			}// else $val=$col_default;
			if($val=='new'){
				$val=$_POST[$prefix."col".$col_id.'_new'];
				check_dir($val);
			}
			if(strstr($val,'core/')) $val='';
			//if($val=='') $val=$col_default;
			//if($val=='') $val='/files/';
			if(!empty($file_dir)){
				if(strlen($val)<strlen($file_dir) || substr($val,0,strlen($file_dir))!=$file_dir) $val='';//$file_dir;
			}
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
					VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($val,1)."')",3,'row_value',__LINE__,__FILE__);
		}
		if($col_type==5 && !empty($_POST[$prefix."col".$col_id])){//пре обработка
			if($_POST[$prefix."col".$col_id]=='invite'){
				$invite_user=$_POST[$prefix."col".$col_id.'invite'];
				invite_user($col_link,$invite_user);
				$_POST[$prefix."col".$col_id]=$invite_user;
			} else if($_POST[$prefix."col".$col_id]=='new'){
				$r=user_reg($_POST[$prefix.'u'.$col_id.'user_login'],$_POST[$prefix.'u'.$col_id.'user_pwl'],$_POST[$prefix.'u'.$col_id.'user_name'],$_POST[$prefix.'u'.$col_id.'user_email'],'','',0,0,$_POST[$prefix.'u'.$col_id.'user_fixedip'],$_POST[$prefix.'u'.$col_id.'user_pwlcode'],0,Array(),false,$_POST[$prefix.'u'.$col_id.'user_session_lifetime'],$_POST[$prefix.'u'.$col_id.'user_session_multy']);
				if($r!=1) $_POST[$prefix."col".$col_id]=0;
				else {
					invite_user($col_link,$GLOBALS["lu"],1);
					$_POST[$prefix."col".$col_id]=$GLOBALS["lu"];
				}
			}
		}
		if($col_type==1 && !empty($_POST[$prefix."col".$col_id])){//пре обработка
			if($_POST[$prefix."col".$col_id]=='new' && $col_link2==0){
				global $rlink;
				$new_prefix=$prefix.'f'.$col_id.'_';
				$tex=0;
				if(!empty($_POST[$new_prefix.'info'])){
					$x=explode('!',$_POST[$new_prefix.'info']);
					$tex=$x[3];
					$modex=$x[4];
				}
				$id6=$x[6];
				if(!empty($id6)){
					seek_rlink($id6);
					$tex=$rlink[$id6]->tex;
				}
				if(empty($tex) && !empty($GLOBALS["lex_table"][$col_link])) $tex=$GLOBALS["lex_table"][$col_link];
				//if(empty($tex)){
				//	$tex=get_tex(0,$modex,$table_id);
				//}
				if(empty($modex) && !empty($GLOBALS["lex_ex"])) $modex=$GLOBALS["lex_ex"];
				if(empty($tex) && !empty($modex)){
					$tex=get_tex(0,$modex,$col_link);//??? нужнается в доп.проверке\
				}				
				//Создаём экземпляр модуля, если его не существует (для связанных модулей)
				if(empty($tex)){
					$tex=add_ex($module_id,'Базовый','Base',getrowval("SELECT table_bold, table_module, table_id FROM main_table WHERE table_module=$module_id AND table_bold=1","table_id"),0);
				}
				//добавляем новый элемент
				if($col_link>0 || !$id6){
					if($col_link<0) $col_link=-$col_link;
					$_POST[$prefix."col".$col_id]=add_row($module_id,$modex,$tex,$col_link,/*$id6*/0,$new_prefix,0);
				} else {
					$col_link=-$col_link;
					$owner_id=$id6;
					while($owner_id!=0){
						seek_rlink($owner_id);
						if(getrowval("SELECT sub_id FROM table_sub WHERE sub_table1=".$rlink[$owner_id]->table." AND sub_table2=$col_link","sub_id")){
							break;
						} else {
							$owner_id=$rlink[$owner_id]->rsub;
						}
					}
					$_POST[$prefix."col".$col_id]=add_row($module_id,$modex,$tex,$col_link,$owner_id,$new_prefix,$owner_id);
				}
			}
		}
		if($col_type==4 && !empty($_POST[$prefix."col".$col_id]) && strstr($_POST[$prefix."col".$col_id],'n') && isset($_POST[$prefix."col".$col_id."ex_name"])){
			$module=safe_sql_input(str_replace('n','',$_POST[$prefix."col".$col_id]),1);
			$new_ex=add_ex($module,$_POST[$prefix."col".$col_id."ex_name"],$_POST[$prefix."col".$col_id."ex_sname"],$_POST[$prefix."col".$col_id."ex_major"],0,$prefix."col".$col_id);
			if($new_ex){
				$_POST[$prefix."col".$col_id]=$module.':'.$new_ex;
				$xr=get_top_row($arow);
				if(!empty($xr->tex)){
					getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=".$xr->tex,1,"ex_group");
				}
				if(!empty($db->Record["ex_ex2"])){
					getrow($db,"SELECT * FROM ex_zone WHERE ex_module=".$db->Record["ex_ex2"],1,"ex_zone");
				}
				if(!empty($db->Record["ex_zone"])){
					$db->query("INSERT INTO ex_zone (ex_zone, ex_module, ex_module2)
							VALUES (".$db->Record["ex_zone"].", $new_ex, $module)",3,'ex_zone');
				}
			} else $_POST[$prefix."col".$col_id]=-3;		
		}
		if($col_type==1 || $col_type==5){
			if($col_link2==0){
				if(!empty($_POST[$prefix."col".$col_id])) $val=$_POST[$prefix."col".$col_id];
				else $val=0;
				//$val=prepend_value($val);
				$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
						VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($val,1)."')",3,'row_value',__LINE__,__FILE__);
			} else if($col_link2==1){
				if(!empty($_POST[$prefix."col".$col_id]) && is_array($_POST[$prefix."col".$col_id])) foreach($_POST[$prefix."col".$col_id] AS $var=>$value)if(!empty($value)){
					$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
							VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($var,1)."')",3,'row_value',__LINE__,__FILE__);
				}
				if(!empty($_POST[$prefix."col".$col_id]) && !is_array($_POST[$prefix."col".$col_id])){
					$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
							VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($_POST[$prefix."col".$col_id],1)."')",3,'row_value',__LINE__,__FILE__);
				}
			}
		}
		if($col_type==2){
			if(!empty($_POST[$prefix."col".$col_id])) $val=1;
			else $val=0;
			if(!empty($col_default) && !isset($_POST[$prefix."col".$col_id]) && $GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]!=-1) $val=$col_default;
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
					VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($val,1)."')",3,'row_value',__LINE__,__FILE__);
		}
		if($col_type==3){
			if(!empty($_POST[$prefix."col".$col_id])) $val=$_POST[$prefix."col".$col_id];
			else if(!empty($_POST[$col_sname])) $val=$_POST[$col_sname];
			else $val='';
			if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1 && !empty($_POST[$prefix."col".$col_id."_serv"])){
				$fos=$_POST[$prefix."col".$col_id."_serv"];//file on server
				if($fos[0]=='/') $fos2=substr($fos,1); else $fos2=$fos;
				if(!file_exists(DOCUMENT_ROOT.$fos) || !check_folder(dirname($fos2),'view')){
					continue;
				} else $val=$fos;//$nname=$fos;
			} else {
				//$nname=upload_file_col($col,$prefix,$val);
			}
			$r=set_col_file($val,$sid,$col,'',$prefix,($table_id==0));
			if($r==-1) continue;
			//if($nname==-1) continue;
			//$nname=prepend_file($nname);
			//if($nname){
			//	$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
			//		VALUES ($id, $table_id, $sid, $col_id, '$nname')",3,'row_value',__LINE__,__FILE__);
			//}
		}
		if($col_type==4){
			if(!empty($_POST[$prefix."col".$col_id])) $val=$_POST[$prefix."col".$col_id];
			else $val=0;
			//$val=prepend_value($val);
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
					VALUES ($id, $table_id, $sid, $col_id, '".safe_sql_input($val,1)."')",3,'row_value',__LINE__,__FILE__);
		}
	}
	//$cols=getall2($db,"SELECT * FROM main_col WHERE col_module=$module_id AND col_table=$table_id AND col_inform=0 AND col_oninsert!='' ORDER BY col_pos, col_bold",$cols,1,'main_col');
	$res='';
	if(empty($GLOBALS["cancel"]))foreach($cols AS $col)if(!empty($col["col_oninsert"]) && !$force){
		$b=backup_globals();
		global $f_type;
		$oldft=$f_type;
		$f_type='onedit';
		$GLOBALS["cur_row"]=$sid;
		$GLOBALS["current"]=$sid;
		//$GLOBALS["cur_col"]=$col["col_sname"];//раньше было $col["col_id"], но на некоторых полях почему-то не срабатывало (возможно дело в кешировании parse_col)		
		unset($GLOBALS["cur_col"]);
		$GLOBALS["cur_col"]->id=$col["col_id"];
		$GLOBALS["cur_col"]->row=$sid;		
		global $ex_ex2;
		$GLOBALS["cur_ex"]=$ex_ex2;
		if(empty($cum)) $GLOBALS["cur_module"]=$module_id; else $GLOBALS["cur_module"]=$cum;
		$GLOBALS["cur_table"]=$table_id;
		$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
		$GLOBALS['li']=$sid;
		$res.=shell_tpl($col["col_oninsert"],0);
		
		$GLOBALS["exit"]=false;
		$GLOBALS["break"]=false;
		$GLOBALS["continue"]=false;
		$GLOBALS['break']=false;
		$GLOBALS['xbreak']=false;
		
		return_globals($b);
		$f_type=$oldft;
	}

	if($flush_posts && !empty($cols)) foreach($cols AS $col) if(isset($_POST[$prefix."col".$col_id])) unset($_POST[$prefix."col".$col_id]);
	if(empty($GLOBALS["cancel"]) && $table_id!=0){
		$db->Record=table_cache($table_id,1);
		//getrow($db,"SELECT * FROM main_table WHERE table_id=$table_id",1,"main_table",__LINE__,__FILE__);
		if(!empty($db->Record["table_onedit"]) && !$force){
			global $f_type;
			$oldft=$f_type;
			$f_type='onedit2';
			$b=backup_globals();
			$GLOBALS["cur_row"]=$sid;
			$GLOBALS["current"]=$sid;
			global $ex_ex2;
			$GLOBALS["cur_ex"]=$ex_ex2;
			//$GLOBALS["cur_module"]=$module_id;
			if(empty($cum)) $GLOBALS["cur_module"]=$module_id; else $GLOBALS["cur_module"]=$cum;
			$GLOBALS["cur_table"]=$table_id;
			$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
			
			$res.=shell_tpl($db->Record["table_onedit"],0);

			$GLOBALS["exit"]=false;
			$GLOBALS["break"]=false;
			$GLOBALS["continue"]=false;
			$GLOBALS['break']=false;
			$GLOBALS['xbreak']=false;			
			
			return_globals($b);
			$f_type=$oldft;
		}
	}
	rebuild_row_index($arow);
	return $res;
}

function prepend_value($value){
	global $prepend_value_checkdbl;
	if(isset($prepend_value_checkdbl[$value])) return $value;
	if(is_object($value) || is_array($value)) return $value;
	
	$value=addslashes($value);
	
	$prepend_value_checkdbl[$value]=1;
	return $value;
}

function input_form($table_id,$module_id,$arow,$acol=0,$ex=0,$modex=0,$force_form=0,$skip_ex=0,$defs=Array(),$prefix='',$allow_add=1,$col_source=0/* 0 - table_col, 1 - ex.param */,$ignore_defs=0){//ex - экземпляр таблицы, modex - экземпляр модуля, force_form если !=0 то игнорируется сol_onform, defs - массив значений по умолчанию, allow_add - возможность отображения компонентов добавления пользователей, строк или модулей (для быстрого редактирования полей)
	global $db,$cum,$input_col_cache,$last_cols;
	$res='';
	$l=$table_id.'.'.$module_id.'.'.$acol.'.'.$col_source;
	if(empty($arow)) $action='add_form'; else $action='edit_form';

	if(!empty($arow)){	
		if($acol==0) $tmp=getall($db,"SELECT * FROM row_value WHERE value_row=$arow",1,"row_value",__LINE__,__FILE__);
		else $tmp=getall($db,"SELECT * FROM row_value WHERE value_row=$arow AND value_col=$acol",1,"row_value",__LINE__,__FILE__);	
		
		if(!empty($tmp)) foreach($tmp AS $tm) $val[$tm["value_col"]]=$tm["value_value"];
		if(!empty($tmp)) foreach($tmp AS $tm) $val2[$tm["value_col"]][]=$tm["value_value"];
	}

	if($acol!=0) $add="col_id=$acol AND "; else $add='';
	
	if(!isset($input_col_cache[$l])){
		if(empty($table_id) || $col_source==1){
			$search_table=$table_id;
			if($col_source==1) $search_table=0;
			$input_col_cache[$l]=getall5($db,"SELECT * FROM main_col WHERE ".$add."col_module=$module_id AND col_table=$search_table AND col_inform=1 ORDER BY col_pos, col_bold","col_cat");
		}
		else	$input_col_cache[$l]=getall5($db,"SELECT * FROM main_col WHERE ".$add."col_table=$table_id AND col_inform=1 ORDER BY col_pos, col_bold","col_cat");
	}
	$last_cols=$input_col_cache[$l];
	$cats=$input_col_cache[$l];
	$lcat=-1000;
	$close_cat=false;
	foreach($cats AS $cat=>$cols)
	foreach($cols AS $col) if(check_col($col["col_id"],'edit')){
		if($lcat!=$cat){
			if($close_cat && !$acol){
				$res.='</div>';
				$close_cat=false;
			}
			if($cat==-1 && !$acol/* && $col['col_inform']*/){
				$close_cat=true;
				$uin=uuin();
				$res.='<h2 OnClick="showhide(\''.$prefix.'additional_cols'.$uin.'\')" style="cursor: pointer;">'.si('point').'Дополнительно</h2>';
				$res.='<div id="'.$prefix.'additional_cols'.$uin.'" style="display: none;">';
			}
		}
		$lcat=$cat;
		foreach($col AS $var=>$value) $$var=$value;
		if($ignore_defs) $col_default='';
		if($col_url) $col_name.='</b> (оставьте пустым для главной страницы)<b>';
		if(!empty($col_hint)) $col_name.='<br><i style="font-weight: normal;">'.str_replace(VSP2,'<br>',$col_hint).'</i>';
		if(isset($defs[$col["col_id"]])) $col_default=$defs[$col["col_id"]];
		$mct=false;//must close tag
		$cof=trim(clear_dblspace($col_onform));
		if(empty($cof) || $force_form==1){
			if($col_type!=2 && $col_type!=1 && $col_type!=3 && ($acol==0/* || $force_form==1*//*0*/) && $force_form==0){
				$res.='<p><b>'.$col_name.'</b>:<br>';
				$mct=true;
			}
			if($col_type==0){
				if($action=='edit_form'){
					$col_default='';
					if(isset($val[$col_id])) $col_default=$val[$col_id];
				}
				$alldone=false;
				if($col_paramlink!=0){
					getrow($db,"SELECT * FROM part_param WHERE param_id=".$col_paramlink,1,"part_param");
					if(!empty($db->Record["param_list"])){
						$pl=$db->Record["param_list"];
						if(strstr($pl,VSP)) $pl=explode(VSP,$pl); else {$pl=Array($pl);}
						$options=Array();
						$i=0;
						foreach($pl AS $p)if(!empty($p)){
							$p=str_replace(chr(10),'',$p);
							$p=str_replace(chr(13),'',$p);
							if(strstr($p,'=')){
								$p=explodeA('=',$p,'','',1);
								$options[$i]->id=$p[1];
								$options[$i]->name=$p[0];
								$options[$i]->selected=($col_default!='' && ($col_default==$p[1] || $col_default==$p[0]));
							} else {
								$options[$i]->id=$p;								
								$options[$i]->name=$p;
								$options[$i]->selected=($col_default!='' && $col_default==$p);
							}
							$i++;
						}
						if(!empty($options)){
							$res.='<select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'"><option value="0">Выберите значение</option>';
							foreach($options AS $option){
								$res.='<option value="'.htmlspecialchars($option->id).'"';
								if($option->selected) $res.=' selected';
								$res.='>'.$option->name.'</option>';
							}
							$res.='</select>';
							$alldone=1;
						}
					}
				}
				if(!$alldone && !empty($col_deflist)){
					$options=explode(VSP2,$col_deflist);
					$res.='<select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'"><option value="0">Выберите значение</option>';
					$complete=false;
					foreach($options AS $option){
						$res.='<option value="'.htmlspecialchars($option).'"';
						if($option==$col_default){
							$res.=' selected';
							$complete=true;
						}
						$res.='>'.$option.'</option>';
					}
					if(!$complete && !empty($col_default)){
						$res.='<option value="'.htmlspecialchars($col_default).'" selected>'.$col_default.'</option>';
					}
					$res.='</select>';
					$alldone=1;
				}
				if(!$alldone){
					$col_default=htmlspecialchars($col_default);
					$res.='<input type="text" name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'" value="'.$col_default.'"';
					if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1 && $col_unique){
						global $id6,$cso;
						if(empty($cso)) $cso=1; else $cso++;
						global $id2;
						$mex=$id2;//$ex;
						if($col_unique==2) $mex=-$id2;//$tex;
						$jf='loadurlB(\'check_unique\',function(x){
							if(window.UniqueCheckId!=null) clearTimeout(window.UniqueCheckId);
							if(x!=\'\'){
								document.getElementById(\''.$prefix.'col'.$col_id.'\').style.backgroundColor=\'#FFEAEA\';
								document.getElementById(\''.$prefix.'colb'.$col_id.'\').style.display=\'\';
							}else{
								document.getElementById(\''.$prefix.'col'.$col_id.'\').style.backgroundColor=\'#F9F9FF\';
								document.getElementById(\''.$prefix.'colb'.$col_id.'\').style.display=\'none\';
							}
						},'.$table_id.',ffv(\'cso'.$cso.'\'),'.$mex.','.$col_id.',ffv(\''.$prefix.'col'.$col_id.'\'),'.$arow.')';
						//document.getElementById(\''.$prefix.'col'.$col_id.'\').value
						//document.getElementById(\'cso'.$cso.'\').value
						$res.=' OnBlur="JavaScript: '.$jf.';"';
						$res.=' OnKeyPress="JavaScript:
							document.getElementById(\''.$prefix.'col'.$col_id.'\').style.backgroundColor=\'#F9F9FF\';
							document.getElementById(\''.$prefix.'colb'.$col_id.'\').style.display=\'none\';
							if(window.UniqueCheckId!=null){ clearTimeout(window.UniqueCheckId); }
							window.UniqueCheckId=setTimeout(function(){'.$jf.'},500);
						">';
						if($col_unique==1) $res.='<div style="display: none; color: #990000; padding-top: 0px; margin-top: 0px; padding-bottom: 15px;" id="'.$prefix.'colb'.$col_id.'">Внимание! Значение должно быть уникальным, в рамках текущей таблицы. Такое значение уже присутствует</div>';
						if($col_unique==2) $res.='<div style="display: none; color: #990000; padding-top: 0px; margin-top: 0px; padding-bottom: 15px;" id="'.$prefix.'colb'.$col_id.'">Внимание! Значение должно быть уникальным, в рамках текущего раздела. Такое значение уже присутствует</div>';
						$res.='<input type="hidden" id="cso'.$cso.'" name="cso'.$prefix.'col'.$col_id.'" value="'.$id6.'">';
					} else $res.='>';
				}
			}
			if($col_type==1){
				if($col_link3!=2) $seek_subt=1; else $seek_subt=0;
				$tmp=Array();
				if($col_link>0){
					$tex=get_tex(0,$modex,$col_link);
					$cl=$col_link;
					$tti=0;
					if($skip_ex==0){
						//echo $tex.' | '.$modex;
						if($tex==0){
							global $id6;
							if(!empty($id6)) $tmp=get_sub($id6,$col_link,1,1,0,0,$seek_subt,$tex,$modex,$table_id,0,$col_deep,1);
							else $tmp=get_sub(0,$col_link,1,1,1,0,$seek_subt,/*$ex*/$tex,$modex,$table_id,0,$col_deep,1);//тут будут выведены все элементы, с игнором экземпляра по сути, надо для того, чтобы выводилась инфа в таблицах при добавлении модулей
							//в случае, если речь идёт об элементе, наследованном от строки, когда tex=по сути строка
						} else {
							$tmp=get_sub(0,$col_link,1,1,1,0,$seek_subt,/*$ex*/$tex,$modex,$table_id,0,$col_deep,1);
						}
					} else $tmp=get_sub(0,$col_link,1,0,1,0,$seek_subt,/*$ex*/$tex,$modex,$table_id,0,$col_deep,1);
				} else if($col_link!=0){
					$cl=-$col_link;
					global $id6;
					//getrow($db,"SELECT * FROM row_owner WHERE owner_id=$id6 AND row_table=$cl");
					//$tti=$db->Record["row_id"];
					$tti=$id6;
					$tex=get_tex(0,$modex,$cl);
					$tmp=get_sub($tti,$cl,1,1,0/*1*/,0,$seek_subt,/*$ex*/$tex,$modex,$table_id,1);
					if(empty($tmp) && !empty($arow)){
								$tti=$arow;
								$tmp=get_sub($tti,$cl,1,1,0/*1*/,0,$seek_subt,/*$ex*/$tex,$modex,$table_id,1,$col_deep,1);
					}
				} else if(!empty($col_speclink)){
					$tmp=parse_var($col_speclink);
					if(is_object($tmp) && isset($tmp->rows)) $tmp=$tmp->rows;
					if(!is_array($tmp)) $tmp=Array();
				}
				if($col_link4==1) $tmp=seek_linked($tmp,$arow);
				if($col_link4==2){
					$tex=get_tex(0,$modex,$cl);
					$tmp=get_sub(0,$cl,1,0,0,0,$seek_subt,$tex,$modex,$table_id,1,$col_deep,1);
				}
				if(empty($tmp) && $action=='edit_form' && !empty($val[$col_id])){
					global $rlink; seek_rlink($val[$col_id]);
					$tmp[]=$rlink[$val[$col_id]];
				}
				$rows=get_vars2($tmp);
				$sres='';
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1 && $col_link!=0 && $col_link3!=1 && $col_link3!=3 && $allow_add){
					global $id6;
					if(check_operation('add',0,$id6,$modex,$cl)){
						$sres='<option value="new">Добавить новое значение</option>';
					}
				}
				if($acol==0 || $force_form==1){
					if($col_link2==0 && (!empty($rows) || !empty($sres))){$res.='<p><b>'.$col_name.'</b>:<br>';$mct=true;}
					else if($col_link2==1 && !empty($rows)) $res.='<p><b>'.$col_name.'</b>:<br>';$mct=true;
				}
				if(!empty($col_deep) && $col_link3==3){
					if(strstr($col_deep,'.')){
						$tmp=explode('.',$col_deep);
						$ntbl=$tmp[count($tmp)-1];
					} else $ntbl=$col_deep;
				} else $ntbl=0;
				if($col_link2==0 && (!empty($rows) || !empty($sres))){
					$res.='<nobr><select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'"';
					//if(!empty($sres)){
					$atex=$tex;
					$amodex=$modex;
					if(empty($atex) && !empty($cl)){
						$atex=getrowval("SELECT * FROM ex_table WHERE ex_table=$cl","ex_id");
						if(!empty($atex)) $amodex=getrowval("SELECT * FROM ex_group WHERE ex_ex1=$atex","ex_ex2");
					}
					$ajax_id=$prefix.'f'.$col_id.'_!'.$cl/*это table_id*/.'!'.$module_id.'!'.$atex.'!'.$amodex.'!'.$skip_ex.'!'.$tti;
					//$res.=' OnChange="JavaScript: val=this.options[this.selectedIndex].value; if(val==\'new\'){ clear(\'addrow'.$prefix.$col_id.'\'); loadurlC(\'add_row_form\',\'addrow'.$prefix.$col_id.'\',\''.$ajax_id.'\'); show(\'addrow'.$prefix.$col_id.'\'); } else hide(\'addrow'.$prefix.$col_id.'\')"';
					if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1) $res.=' OnChange="change_select(\''.$prefix.$col_id.'\',\''.$ajax_id.'\',this);"';
					//}
					if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1) $res.= ' style="margin-right: 3px;"';
					$res.='><option value="0">Выберите значение</option>';
					if(empty($col_default)) $col_default=0;
					if($action=='edit_form'){
						if(!empty($val[$col_id])) $col_default=$val[$col_id];
					}
					$level=seek_max_level($rows);
					if($col_link3!=3) $level=0;
					$res.=options($rows,' ',$col_default,1,0,0,0,'',Array(),10000,0,$col_link3,$level,$ntbl,1);
					/*if(!empty($atex)) */$res.=$sres; //эта проверка нужна была для исключения возможности добавления строки без создания экземпляра
					$res.='</select>';
					$ustyle='display: none;';
					$uhref='';
					if(!empty($col_default) && is_numeric($col_default)){
						global $zone_url;
						$r=get_rlink($col_default);
						if(!empty($r) && check_row($col_default,$r->table,get_ex2($r->tex),"edit",$r->user,$r->users)){
							$uhref=$zone_url.'/'.get_admin_url($col_default);
							$ustyle='';
						}
					}
					if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1) $res.='<a href="'.$uhref.'" id="hrefer'.$prefix.$col_id.'" style="'.$ustyle.'">'.si('arrow',5,0,'Перейти к объекту').'</a></nobr>';
					if(!empty($sres)){
						$res.='<div id="addrow'.$prefix.$col_id.'" style="display: none; padding: 20px; padding-top: 5px; margin-right: 17px; background-color: #F9F9F9; border-left: 1px solid #999999; border-right: 1px solid #999999; border-bottom: 1px solid #999999;"></div>';
					}
				} else if($col_link2==0 && empty($rows)) $res.='<input type="hidden" name="'.$prefix.'col'.$col_id.'" value="0">';
				else if($col_link2==1){
					//checkbox
					$level=seek_max_level($rows);
					if($col_link3!=3) $level=0;
					$res.='<input type="hidden" name="'.$prefix.'col'.$col_id.'_check" value="1">';
					if($action=='edit_form' && !empty($val2[$col_id])) $res.=checkbox($rows,'',$prefix.'col'.$col_id,$col_link3,$val2[$col_id],0,$level,$ntbl);
					else {
						if($col_default!='') $res.=checkbox($rows,'',$prefix.'col'.$col_id,$col_link3,explode(',',$col_default),0,$level,$ntbl);
						else $res.=checkbox($rows,'',$prefix.'col'.$col_id,$col_link3,Array(),0,$level,$ntbl);
					}
				}
			}
			if($col_type==2){
				$add='';
				if($action=='edit_form' && !empty($val[$col_id]) && $val[$col_id]==1) $add=' checked';				
				if($col_default=='1' && $action!='edit_form') $add=' checked';//для возможности установки значения по умолчанию в чекбоксе
				$res.='<input type="hidden" name="'.$prefix.'col'.$col_id.'_check" value="1">';//без этого при обновлении информации берётся информация из backup_row, т.к. считается что если галочки нет, то эта переменная в POST-е не empty, а !isset
				if($acol==0)$res.='<p><label style="cursor: pointer"><input type="checkbox" name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'" class="button"'.$add.'><b>'.$col_name.'</b></label></p>';
				else $res.='<input type="checkbox" name="'.$prefix.'col'.$col_id.'" class="button"'.$add.'>';
			}
			if($col_type==3){
				// Здесь должно быть предложение к выбору файла из файлового менеджера
				$cfs='';
				if($file_maxsize>0) $cfs=' (макс.размер '.$file_maxsize.'кб)';
				$res.='<p><b>'.$col_name.$cfs.'</b>:<br>';
				$mct=true;
				if($action=='edit_form' && $GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1 && !empty($val[$col_id]) && file_exists(DOCUMENT_ROOT.$val[$col_id])){
					global $ex_module,$module_id/*,$table_id*/,$ex_ex2;
					if(!empty($col_onshow) && !empty($table_id)){
						$b=backup_globals();
						$GLOBALS["cur_row"]=$arow;
						//$GLOBALS["cur_col"]=/*$col_sname*/$col_id;
						unset($GLOBALS["cur_col"]);
						$GLOBALS["cur_col"]->id=$col_id;
						$GLOBALS["cur_col"]->row=$arow;
						$GLOBALS["cur_ex"]=$ex_ex2;//$ex_module;
						//$GLOBALS["cur_module"]=$module_id;
						if(empty($cum)) $GLOBALS["cur_module"]=$module_id; else $GLOBALS["cur_module"]=$cum;
						$GLOBALS["cur_table"]=$table_id;
						$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
						$res.=shell_tpl($col_onshow,0);
						return_globals($b);
					}
					global $id,$id2,$id3,$id4,$id5,$id6,$id7,$base_root;
					$res.='<div>Размер: '.(round(filesize(DOCUMENT_ROOT.$val[$col_id])/1000)).'кб';
					if(!empty($table_id) && $acol==0) $res.=' ( <a href="'.$base_root.$val[$col_id].'" target="_blank">загрузить</a> / <a href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$id3.'&amp;id4='.$id4.'&amp;id5='.$id5.'&amp;id6='.$id6.'&amp;id7='.$id7.'&amp;action=edit_form&amp;act2=delfile&amp;file='.$col_id.'">удалить</a> )';
					else if(empty($table)) $res.=' ( <a href="'.$base_root.$val[$col_id].'" target="_blank">загрузить</a> / <a href="mod_main?id='.$id.'&amp;id2='.$id2.'&amp;action=delfile&amp;file='.$col_id.'">удалить</a> )';
					$res.='</div>';
					//текущая информация о файле (с учётом его вывода как в таблице)
					//и ссылка "удалить"
				}
				$res.='<input type="file" name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'">';
				if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1){
					$res.='<div style="display: none;" id="'.$prefix.'col'.$col_id.'_block"><input type="hidden" name="'.$prefix.'col'.$col_id.'_serv" id="'.$prefix.'col'.$col_id.'_serv" value="">Выбран файл на сервере - <span id="'.$prefix.'col'.$col_id.'_info"></span><br><span class="link" OnClick="up_from_pc(\''.$prefix.'col'.$col_id.'\');">Загрузить файл с компьютера</span> или <span class="link" OnClick="show_elfinder(function(url){up_from_serv(url,\''.$prefix.'col'.$col_id.'\');});">Выбрать другой файл</span></div>';
					$res.='<div align="left" style="width: 99%; margin-top: -7px;" id="'.$prefix.'col'.$col_id.'_browse"><span class="link" OnClick="show_elfinder(function(url){up_from_serv(url,\''.$prefix.'col'.$col_id.'\');});">или открыть с сервера</span></div>';
				}
			}
			if($col_type==4){
				$res.='<select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'"';
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1) $res.=' OnChange="$(\'#'.$prefix.'addex\').html(\'\'); val=this.options[this.selectedIndex].value; if(val.indexOf(\'n\')!=-1){ clear(\''.$prefix.'addex\'); loadurlC(\'add_ex_form\',\''.$prefix.'addex\',val.replace(\'n\',\'\'),\''.$prefix.'col'.$col_id.'\'); show(\''.$prefix.'addex\'); } else hide(\''.$prefix.'addex\')"';
				$res.='>';
				$col_default=0;
				if($action=='edit_form' && !empty($val[$col_id])) $col_default=$val[$col_id];
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1 && $allow_add) $add_new=1; else $add_new=0;
				$res.=module_select($module_url,$module_type,$col_default,0,0,2,$add_new);
				$res.='</select>';
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1) $res.='<div id="'.$prefix.'addex" style="display: none; padding: 20px; padding-top: 5px; margin-right: 17px; background-color: #F9F9F9; border-left: 1px solid #999999; border-right: 1px solid #999999; border-bottom: 1px solid #999999;"></div>';
			}
			if($col_type==5){
				$res.='<select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'" OnChange="JavaScript:';
				if(check_group($col_link,'invite'))
					$res.=' var d1=document.getElementById(\''.$prefix.'col'.$col_id.'invite\');					
						d1.style.display=\'none\'; 
						if(this.options[this.selectedIndex].value==\'invite\') d1.style.display=\'\';';					
					if(check_user(0,'reg'))
						$res.='var d2=document.getElementById(\''.$prefix.'col'.$col_id.'new\');
							d2.style.display=\'none\';
							if(this.options[this.selectedIndex].value==\'new\') d2.style.display=\'\';';
				$res.='">';
				$col_default=0;
				if($action=='edit_form' && !empty($val[$col_id])) $col_default=$val[$col_id];
				$res.='<option value="0">Выберите пользователя</option>';
				$res.=user_select($col_link,$col_default);
				if(check_group($col_link,'invite') && $allow_add && !empty($col_link)){
					$res.='<option value="invite">Пригласить пользователя</option>';
					if(check_user(0,'reg')) $res.='<option value="new">Создать пользователя</option>';
				}
				$res.='</select>';
				if(check_group($col_link,'invite')){
					$res.='<div style="display: none;" id="'.$prefix.'col'.$col_id.'invite" class="subdiv"><p>Выберите пользователя: <br><select name="'.$prefix.'col'.$col_id.'invite">'.select_users(0,0,0,$col_link).'</select></p></div>';	
					if(check_user(0,'reg')){
							$res.='<div style="display: none;" id="'.$prefix.'col'.$col_id.'new" class="subdiv">';
							$res.=add_user_form('',$prefix.'u'.$col_id);
							$res.='</div>';	
					}
				}
			}
			if($col_type==6){
				if($action=='edit_form'){
					$col_default='';
					if(isset($val[$col_id])) $col_default=$val[$col_id];
				}
				if(!empty($file_dir) && empty($col_default)) $col_default=$file_dir;
				if($col_default=='') $col_default='/files/';
				$col_default=htmlspecialchars($col_default);
				//$res.='<input type="text" name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'" value="'.$col_default.'">';
				if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1){
					$res.='<nobr>';
				}
				$res.='<select name="'.$prefix.'col'.$col_id.'" id="'.$prefix.'col'.$col_id.'" OnChange="';
				$res.='if($(this).val()==\'new\') $(\'#'.$prefix.'col'.$col_id.'_new\').show(); else $(\'#'.$prefix.'col'.$col_id.'_new\').hide();';
				if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1){
					$res.='if($(this).val()==\'new\' || !$(this).val()) $(\'#'.$prefix.'col'.$col_id.'_folder\').hide(); else $(\'#'.$prefix.'col'.$col_id.'_folder\').show();';
				}
				$res.='">';
				$res.='<option value="">Нет</option>';
				$match=false;
				$start_dir=$file_dir;
				if(empty($start_dir)) $start_dir='files';
				if($start_dir[0]=='/') $start_dir=substr($start_dir,1);
				if($start_dir[strlen($start_dir)-1]=='/') $start_dir=substr($start_dir,0,strlen($start_dir)-1);
				$dirs=scan_dir($start_dir,Array(),1,1,0,0,5,0,0);
				$has_option=false;
				foreach($dirs AS $dir)if(check_folder($dir,'view'))if(substr($dir,0,12)!='files/editor' && substr($dir,0,8)!='files/js' && substr($dir,0,12)!='files/deploy'){
					if($dir[0]!='/') $dir='/'.$dir;
					if($dir[strlen($dir)-1]) $dir.='/';
					if($dir==$file_dir) continue;
					if($dir==$col_default){
						$match=true;
						$add=' selected';
					} else $add='';
					$res.='<option value="'.$dir.'"'.$add.'>'.substr($dir,7,strlen($dir)-8).'</option>';
					$has_option=true;
				}
				if(!$match && !empty($val[$col_id]) && $action=='edit_form'){
					$res.='<option value="'.$col_default.'" selected>'.substr($col_default,7).'</option>';
					$has_option=true;
				}
				$res.='<option value="new">Создать новую</option>';
				$res.='</select>';
				if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1){
					$res.='<img style="'.(empty($val[$col_id])?'display:none;':'').' cursor: pointer;" id="'.$prefix.'col'.$col_id.'_folder" rel="'.$prefix.'col'.$col_id.'" src="'.$GLOBALS['base_root'].'/files/editor/classic/file.png" width="14" height="14" hspace="6" border="0" OnClick="show_elfinder(function(){},$(\'#\'+$(this).attr(\'rel\')).val());">';
					$res.='</nobr>';
				}
				
				//$res.='<div style="float: right; width: 26px;"><img src="'.$base_root.'/files/editor/classic/file.png" width="14" height="14" hspace="6" border="0" OnClick="show_elfinder();" style="cursor: pointer;"></div>';
				
				$res.='<div id="'.$prefix.'col'.$col_id.'_new" style="'./*($has_option?'display: none;':'')*/'display: none;'.'"><input type="text" name="'.$prefix.'col'.$col_id.'_new" value="'.$file_dir.'new_folder/"></div>';
			}
		} else {
			$b=backup_globals();
			global $f_type;
			$oldft=$f_type;
			$f_type='onform';
			$GLOBALS["cur_row"]=$arow;
			unset($GLOBALS["cur_col"]);
			$GLOBALS["cur_col"]->id=$col_id;
			$GLOBALS["cur_col"]->row=$arow;
			$GLOBALS["glob_prefix"]=$prefix;
			global $ex_module,$ex_ex2;
			$GLOBALS["cur_ex"]=$ex_ex2;
			//$GLOBALS["cur_module"]=$module_id;
			if(empty($cum)) $GLOBALS["cur_module"]=$module_id; else $GLOBALS["cur_module"]=$cum;
			$GLOBALS["cur_table"]=$table_id;
			$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
			//if($col_onform=='[form.textarea height=200]') echo 'col: '.$col_sname.' - row: '.$arow.' - prefix: '.$prefix.' - module: '.$GLOBALS["cur_module"].' - table: '.$GLOBALS["cur_table"];
			//$GLOBALS["cur_table"]=0;
			
			//echo $col_sname.' - '.$arow.' - '.$col_onform.'<br>';
			
			$tmp=shell_tpl($col_onform,0);
			
			$GLOBALS["exit"]=false;
			$GLOBALS["break"]=false;
			$GLOBALS["continue"]=false;
			$GLOBALS['break']=false;
			$GLOBALS['xbreak']=false;			
			
			if($col_type!=2 && $col_type!=1 && $col_type!=3 && $acol==0 && !empty($tmp)){
				$res.='<p><b>'.$col_name.'</b>:<br>';
				$mct=true;
			}
			$res.=$tmp;
			return_globals($b);
			$f_type=$oldft;
		}
		if($mct) $res.='</p>';
	} else {
		if($action=='edit_form' && !empty($val[$col["col_id"]])){
			$res.='<input type="hidden" name="reset['.$prefix.$col["col_id"].']" value="1">';//тут было эхо, потом я перепломбировал это каким-то макаром и позже пофиксил в RES. не тестил
		}
	}
	if($close_cat){
		$res.='</div>';
		$close_cat=false;
	}
	return $res;
}

function add_row($module_id,$modex,$tex,$table_id,$id6,$prefix,$ro_owner=0){
	global $db,$user,$rlink,$glob_prefix;
	if(!check_operation('add',0,$id6,$modex,$table_id)) return 0;
	$r=backup_globals();
	$GLOBALS["f_action"]='add';
	$GLOBALS["cur_ex"]=$modex;
	
	$old_glob_prefix=$glob_prefix;
	$glob_prefix=$prefix;

	$ro_owner3=$ro_owner;
	$table_id3=$table_id;
	if(!empty($id6) && empty($ro_owner)){
		$ro_owner3=$id6;
		//$table_id3=$up_table_id;	//??		
	}
	if(!empty($ro_owner)){
		seek_rlink($ro_owner);
		$table_id3=$rlink[$ro_owner]->table;
	}

	del_cache('rows',$table_id.'.'.$tex);
	if(empty($id6)) $row_sub=0; else {
		$row_sub=$id6;
		del_cache('row',$row_sub);
	}
	if(!empty($ro_owner3)){
		del_cache('row',$ro_owner3);
	}
	$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user, row_uin, creation_date, modified_date)
			VALUES ($module_id, $table_id, $tex, $row_sub, ".$user->id.", '".uuin()."', '".date('Y-m-d H:i:s')."', '".date('Y-m-d H:i:s')."')",3,'main_row');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	$ro_sub=0;if(!empty($id6)) $ro_sub=$id6;
	getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex IN (".get_exes($ro_owner3,$tex).") AND row_table=$table_id AND owner_id=$ro_owner3 AND owner_table=$table_id3 AND ro_sub=$ro_sub AND owner_module=$module_id",1,'row_owner');
	if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
	if(empty($id6)) $ro_sub=0; else $ro_sub=$id6;
	$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, ro_sub, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
			VALUES ($pos, $tex, $sid, $ro_sub, $module_id, $table_id, $ro_owner3, $table_id3, $module_id, ".$user->id.")",3,'row_owner');				
			
	$GLOBALS["cancel"]=false;
	echo insert_values($table_id,$module_id,$sid,0,$prefix);
	if($GLOBALS["cancel"]){
		del_row($sid);
		$sid=0;
		if($GLOBALS["cancel"]!=-1){
			if($GLOBALS["cancel"]==1) echo '<div align="center"><h2 style="color:#FF0000;">Добавление не удалось</h2></div>';
			else echo '<div align="center"><h2 style="color:#FF0000;">Добавление не удалось по причине: '.$GLOBALS["cancel"].'</h2></div>';
		}
	}					
	$GLOBALS["backrow"]=0;
	$GLOBALS["f_action"]='';
	return_globals($r);
	$glob_prefix=$old_glob_prefix;
	add_row_index($sid);
	return $sid;
}

function seek_row_type($row){	//превращает row в rows
	global $db, $rlink;
	if(!empty($row->table)) return $row;
	if(empty($row)) return $row;
	foreach($row AS $r){break;}
	//if(empty($r->table)) return $row;
	stf('seek_row_type');
	if(!is_object($r)){//это может затормаживать процесс (необходимо для преобразования Е4 строк в Е5)
		foreach($row AS $var=>$value){
			if(!is_numeric($value)) return $row;
			seek_rlink($value);
			$rows->rows[$var]=$rlink[$value];
		}
	} else $rows->rows=$row;
	if(!isset($r->table)){
		if(is_object($r) && isset($r->id)) seek_rlink($r->id);
		else  seek_rlink($r);
		if(is_object($r) && isset($r->id)) $r=$rlink[$r->id];
		else $r=$rlink[$r];
	}

	//это специальная защита от прокосяченных объектов, у которых ID есть, а POS нет (например отмеченные значение поля, связной таблицы col_link)
	foreach($rows->rows AS $r2){break;}
	if(!isset($r2->pos) && isset($r2->id)){
		foreach($rows->rows AS $var=>$value){
			seek_rlink($value->id);
			$rows->rows[$var]=$rlink[$value->id];
		}
	}
	
	if(!is_object($r)) return $row;
	$rows->table=$r->table;
	$ex=get_ex($r->id,$r->table);
	$rows->ex=$ex;
	$rows->owner=$r->owner;
	$rows->tex=get_tex($r->owner,$ex,$r->table);
	$db->Record=table_cache($r->table);
	//getrow($db,"SELECT * FROM main_table WHERE table_id=".$r->table,1,"main_table",__LINE__,__FILE__);
	$stable=$db->Record["table_sname"];
	$rows->stable=$stable;
	etf('seek_row_type');
	return $rows;
}

function row_limit($rows,$name,$start,$end,$in_param){
	stf('row_limit');
	global $filter,$p_par;
	do_log('Limit rows '.$name);
	if((empty($rows->table) || !isset($rows->ex)) && is_array($rows) && !empty($rows)) $rows=seek_row_type($rows);
	else {
		if(is_object($rows) && isset($rows->rows) && is_array($rows->rows) && !isset($rows->ex)) $rows=seek_row_type($rows->rows);
	}
	if(empty($rows->table)){
		$res='';
		if($in_param) $p_par[$name]=$res;
		etf('row_limit');
		return '';
	}
	$res->table=$rows->table;
	$res->ex=$rows->ex;
	$res->owner=$rows->owner;
	$res->rows=Array();
	if(is_object($start)) $start=parse_var($start);
	if(is_object($end)) $end=parse_var($end);
	if(empty($start)) $start=0;
	for($i=$start;$i<$end;$i++)if(!empty($rows->rows[$i])) $res->rows[]=$rows->rows[$i];
	//$filter[$name][$rows->table][$rows->ex][$rows->owner]=$res;
	if(!$in_param) $filter[$name][$rows->table][$rows->ex][$rows->owner]=$res;
	else $p_par[$name]=$res;
	etf('row_limit');
}

function pos_in_rows($rows,$id){
	foreach($rows AS $r) if($r->id==$id){
		return $r->pos;
	}
	return 0;
}

function pos_in_rows2($rows,$rid){
	foreach($rows AS $r) if($r->rid==$rid){
		return $r->pos;
	}
	return 0;
}

function row_order($rows,$name,$col,$sort,$in_param=0){	//sort - ASC, DESC, RAND; col - col_sname
	stf('row_order');
	global $filter,$db,$p_par;
	if(empty($rows->table) && is_array($rows) && !empty($rows)) $rows=seek_row_type($rows);
	if(empty($rows->table)){
		$res='';
		if($in_param) $p_par[$name]=$res;
		etf('row_order');
		return '';
	}
	do_log('Order rows '.$name);
	$res->table=$rows->table;
	if(isset($rows->ex)) $res->ex=$rows->ex;//непоняткая ошибка была здесь
	$res->owner=$rows->owner;
	$res->rows=Array();
	$valsX=Array();
	if(is_array($col) && isset($col["col_sname"])) $col=$col["col_sname"];
	if(is_array($col)) $col=implode(',',$col);
	if(strstr($col,',')){
		$col=explode(',',$col);
		$use_id=false; $use_pos=false; $use_owner=false; $cs='';$i=0; $si=0; $sp=0; $so=0; $ccol=count($col);
		foreach($col AS $c)if($c!='id' && $c!='pos' && $c!='owner'){
			$i++;
			if(!empty($cs)) $cs.=','; $cs.="'".$c."'";
		} else if($c=='id') {$use_id=true; $si=$i; $i++;}
		else if($c=='pos') {$use_pos=true; $sp=$i; $i++;}
		else if($c=='owner') {$use_owner=true; $so=$i; $i++;}		
		if(!empty($cs)) $cols=getall($db,"SELECT * FROM main_col WHERE col_table=".$rows->table." AND col_sname IN ($cs)",1,"main_col",__LINE__,__FILE__);
		$tcols=Array();
		if(!empty($cols)) foreach($cols AS $cc){
			$tcols[$cc["col_sname"]]=$cc["col_id"];
		}		
		$xcol=$col;
		$col=Array(); 
		foreach($xcol AS $c){
			if($c=='id'){
				$col2[]='id';
			} else if($c=='pos'){
				$col2[]='pos';
			} else if($c=='owner' || $c=='own'){
				$col2[]='owner';
			} else if($c=='user' || $c=='usr'){
				$col2[]='user';
			} else {
				if(!empty($tcols[$c])){
					$col2[]=$tcols[$c];
					$col[]=$tcols[$c];
				}
			}
		}
		$cs=implode(',',$col);
		//echo implode(',',$col2);
		$sid='';
		foreach($rows->rows AS $r){if(!empty($sid)) $sid.=','; $sid.=$r->id;}
		if(empty($sid)){$filter[$name][$rows->table][$rows->ex][$rows->owner]=$res; etf('row_order'); return '';} else $sid='('.$sid.')';
		if(!empty($cs)) $tmp=getall($db,"SELECT * FROM row_value WHERE value_row IN $sid AND value_col IN ($cs) AND value_table=".$rows->table,1,"row_value",__LINE__,__FILE__);
		global $vcache,$vcache2;
		$tmp2=Array();
		if(!empty($tmp)) foreach($tmp AS $tm){
			$tmp2[$tm["value_row"]][$tm["value_col"]]=$tm["value_value"];
			$vcache[$tm["value_col"]][$tm["value_row"]][$tm["value_id"]]=$tm;
			$vcache2[$tm["value_col"]][$tm["value_row"]]=$tm;
		}
		$last=Array();
		//if(!empty($tmp2)) foreach($tmp2 AS $row=>$cols){
		foreach($rows->rows AS $row){
			$tmp='';
			if(!is_object($row)){
				$rid=$row;
			} else $rid=$row->id;
			foreach($col2 AS $c){
				if(!empty($tmp)) $tmp.='.';				
				if($c=='id') $tmp.=$rid;
				else if($c=='owner'){
					if(is_object($row)) $tmp.=$row->owner;
					else {
						global $rlink; seek_rlink($row);
						$tmp.=$rlink[$row]->owner;
					}
				}
				else if($c=='user'){
					if(is_object($row)) $tmp.=$row->user;
					else {
						global $rlink; seek_rlink($row);
						$tmp.=$rlink[$row]->user;
					}
				}
				else if($c=='pos') {
					if(is_object($row)) $tmp.=pos_in_rows2($rows->rows,$row->rid)/*$row->pos*/;
					else {
						global $rlink; seek_rlink($row);
						$tmp.=$rlink[$row]->pos;
					}
				}
				else if(!empty($tmp2[$rid][$c])){
					$tmp.=$tmp2[$rid][$c];
					//echo $c.' - '.$rid.' - '.$tmp2[$rid][$c].'<br>';
				}
			}
			if(!empty($tmp)) $valsX[$tmp][]=$rid; else $last[]=$rid;
		}
		//}
		//foreach($tmp AS $tm) $vals[$tm["value_value"]][]=$tm["value_row"];
	} else {
		if($col!='id' && $col!='' && $col!='pos' && $col!='owner' && $col!='own' && $col!='usr' && $col!='user'){
			getrow($db,"SELECT * FROM main_col WHERE col_table=".$rows->table." AND col_sname='$col'",1,"main_col",__LINE__,__FILE__);
			if(!empty($db->Record["col_id"])){
				$col_id=$db->Record["col_id"];
				$sid='';
				foreach($rows->rows AS $r){if(!empty($sid)) $sid.=','; {if(is_object($r) && isset($r->id)) $sid.=$r->id; else $sid.=$r;} }
				if(empty($sid)){$filter[$name][$rows->table][$rows->ex][$rows->owner]=$res; etf('row_order'); return '';} else $sid='('.$sid.')';
				$tmp=getall($db,"SELECT * FROM row_value WHERE value_row IN $sid AND value_col=$col_id AND value_table=".$rows->table,1,"row_value",__LINE__,__FILE__);
				$arow=Array();
				global $vcache2,$vcache;
				foreach($tmp AS $tm){
					if(empty($tm["value_value"])){ $tm["value_value"]=-1000;}
					else {
						$vcache[$tm["value_col"]][$tm["value_row"]][$tm["value_id"]]=$tm;
						$vcache2[$tm["value_col"]][$tm["value_row"]]=$tm;
					}
					$valsX[$tm["value_value"]][]=$tm["value_row"];
					$arow[$tm["value_row"]]=1;
				}
				if(count($rows->rows)>count($tmp)){ //это на случай если некоторых полей ещё нет в базе (актуально при добавлении столбца уже после занесения данных)
					foreach($rows->rows AS $r){
						if(is_object($r) && isset($r->id)) $n=$r->id; else $n=$r;
						if(empty($arow[$n])) $valsX[-1000][]=$n;
					}
				}
			}// else echo $rows->table.' '.$col.'!<br>';
		} else {
			if($col=='owner' || $col=='own'){
				foreach($rows->rows AS $row) $valsX[$row->owner][]=$row->id;
			} else if($col=='usr' || $col=='user'){
				foreach($rows->rows AS $row) $valsX[$row->owner][]=$row->user;
			} else if($col=='pos'){
				foreach($rows->rows AS $row) $valsX[pos_in_rows2($rows->rows,$row->rid)/*$row->pos*/][]=$row->id;
			} else {
				if($col!='') foreach($rows->rows AS $row) $valsX[$row->id][]=$row->id;
				else foreach($rows->rows AS $row) $valsX[$row->id]=$row->id;
			}
		}
	}
	$rs=Array();
	/*for($i=0;$i<count($rows->rows);$i++)if(!empty($rows->rows[$i])){
		if(is_object($rows->rows[$i])) $rs[$rows->rows[$i]->id]=$i;
		else $rs[$rows->rows[$i]]=$i;
	}*/
	foreach($rows->rows AS $i=>$rsi){
		if(is_object($rows->rows[$i])) $rs[$rows->rows[$i]->id]=$i;
		else $rs[$rows->rows[$i]]=$i;
	}
	if($sort=='ASC') ksort($valsX);
	if($sort=='DESC') krsort($valsX);
	if($sort=='RAND'){srand(); shuffle($valsX);}
	if(!empty($last)){
		//foreach($last AS $l) $valsX[-1000][]=$l;
		$valsX=array_merge_recursive($valsX,$last);
	}
	$ui=0;
	if($col!=''){
		foreach($valsX AS $var=>$value) foreach($value AS $var2=>$value2)if(isset($rs[$value2])){
			$res->rows[$ui]=$rows->rows[$rs[$value2]];
			if(!empty($res->rows[$ui]->sub)) $res->rows[$ui]->sub=row_order($res->rows[$ui]->sub,$name,$col,$sort,2);
			$ui++;
		}
	}else foreach($valsX AS $var=>$value){
		$res->rows[$ui]=$rows->rows[$rs[$value]];
		if(!empty($res->rows[$ui]->sub)) $res->rows[$ui]->sub=row_order($res->rows[$ui]->sub,$name,$col,$sort,2);
		$ui++;
	}
	if($in_param==0){
		if(!isset($rows->ex))$filter[$name][$rows->table][0][$rows->owner]=$res;// по идее тут надо вычислять средний ex для строк
		else $filter[$name][$rows->table][$rows->ex][$rows->owner]=$res;
	}
	if($in_param==2){
		etf('row_order');
		return $res->rows;
	}
	if($in_param==3){
		etf('row_order');
		return $res;
	}
	if($in_param==1) $p_par[$name]=$res;
	etf('row_order');
}

function row_filter($rows,$name,$condition,$in_param=0,$seek_sub=0,$as_linear=0){	//sort - ASC, DESC; col - col_sname, in_param - если 0, то рез. записывается в таблицу, иначе в param, as_linear - воспринимает входящий массив как линейный, иначе - как иерархичный
	// в новой версии condition это набор ops
	stf('row_filter');	
	global $filter,$db,$p_par,$no_cache;
	//if($condition=="cur.url=='mksm.html'") echo '123';
	do_log('Filter rows '.$name);
	//$rows=dc($rows);
	if(empty($rows->table) && is_array($rows) && !empty($rows)) $rows=seek_row_type($rows);
	//if(empty($rows->table)) return $rows;
	if(empty($rows->table) && is_array($rows)){
		$tmp=$rows;
		unset($rows);
		$rows->rows=$tmp;
		$rows->table=0;
		$rows->ex=0;
		$rows->tex=0;
		$rows->stable=0;
		$rows->owner=0;
	}
	if(isset($rows->rows)) $r=dc($rows->rows); else $r=dc($rows);//вот этого имхо не надо
	$rows2=Array();
	if(empty($r)){
		etf('row_filter');
		if($in_param==2) return $rows2;
		return '';
	}
	$res->table=$rows->table;
	if(!empty($res->ex)) $res->ex=$rows->ex;
	if(!empty($res->tex)) $res->tex=$rows->tex;
	$res->owner=$rows->owner;
	if(!empty($res->stable)) $res->stable=$rows->stable;
	$res->rows=Array();
	$g=backup_globals();
	$GLOBALS["cur_table"]=$rows->table;
	if(!empty($rows->ex)) $GLOBALS["cur_ex"]=$rows->ex;
	$GLOBALS["in_tree"]=1;

	if(!empty($GLOBALS["current"])){
		if(empty($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		$GLOBALS["up"][$GLOBALS["spec_step"]]=$GLOBALS["current"];
		$GLOBALS["spec_step"]++;
		$upper=true;
	} else $upper=false;
	if(empty($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
	$GLOBALS["spec_step2"]++;
	$first=true;
	if(!empty($r) && $as_linear){
		$r=collect_rows3($r);
	}
	$i=0;

	if(!empty($r)) foreach($r AS $var=>$rs){
		$b=backup_globals();
		
		//if(!isset($GLOBALS["spec_step"])) $GLOBALS["spec_step"]=0;
		//if(!isset($GLOBALS["spec_step2"])) $GLOBALS["spec_step2"]=0;
		//if($step==0) $GLOBALS["spec_step"]++;
		//if($step==0) $GLOBALS["spec_step2"]++;
		
		if($first){
			global $tree_vars,$tv_cache;
			$tree_vars[$GLOBALS["spec_step2"]]=Array();
		}
	
		if(is_object($rs)){
			if(empty($GLOBALS["cur_type"])) $GLOBALS["cur_type"]='';
			if(!empty($rs->table)) $GLOBALS["cur_table"]=$rs->table;			
			$GLOBALS["current"]=$rs->id;
			if(!empty($rs->type)){
				$GLOBALS["cur_type"]=$rs->type;
			} else $GLOBALS["cur_type"]='row';
			$GLOBALS["up"][$GLOBALS["spec_step"]]=$rs->id;
			if(!empty($rs->type)) $GLOBALS["up2"][$GLOBALS["spec_step"]]=$rs->type;
			else $GLOBALS["up2"][$GLOBALS["spec_step"]]='';
		}// else $GLOBALS["current"]=$tre; //опасность
		$GLOBALS["up3"][$GLOBALS["spec_step"]]=$var;
		$GLOBALS["up4"][$GLOBALS["spec_step"]]=$i;
		$GLOBALS["up5"][$GLOBALS["spec_step"]]=$rs;
		$GLOBALS["index"]=$i;
		$GLOBALS["count"]=count($r);
		$GLOBALS["cur_var"]=$var;
		$GLOBALS["for_value"]=$rs;
	
		if(!empty($rs->id)) $GLOBALS["current"]=$rs->id;
		if(do_if($condition)){
			if(!empty($rs->sub) && $seek_sub) $rs->sub=row_filter($rs->sub,'',$condition,2,1);
			$rows2[]=$rs;
			/*$res->rows[]=$rs;*/
		}
		//if(count($r)==1144/* && $first*/) echo count($tree_vars[$GLOBALS["spec_step2"]]).'<br>';
		
		if($first){//оптимизация вытаскивания значений (нужно как-то защитить от повторных вытаскиваний)
			if(tree_optimize($rs,$r)) $first=false;
		}
		return_globals($b);
		$i++;
	}
	$res->rows=$rows2;
	if($upper) $GLOBALS["spec_step"]--;
	if($upper) $GLOBALS["spec_step2"]--;
	return_globals($g);
	
	$GLOBALS["in_tree"]=0;

	if($in_param==0) $filter[$name][$rows->table][$rows->ex][$rows->owner]=$res;
	if($in_param==1) $p_par[$name]=$res;
	if($in_param==2){etf('row_filter'); return $rows2;}
	if($in_param==3){etf('row_filter'); return $res;}
	/*else {
		$p_par[$name]=$res;
	}*/
	etf('row_filter');
}

function seek_linked($rows,$row,$step=0,$c=Array()){
	global $db,$rlink;
	$res=Array();
	$i=0;
	if(empty($row)) return $res;
	if($step==0){
		//$tbls=collect_tables($rows);
		//if(empty($tbls)) return $res;
		//echo "SELECT * FROM main_col WHERE col_link IN (".implode(',',$tbls).") AND col_type=1".'<br>';
		//$cls=getall3($db,"SELECT * FROM main_col WHERE col_link IN (".implode(',',$tbls).") AND col_type=1","col_id");
		//echo var_dump($cls);
		seek_rlink($row);
		$cls=getall3($db,"SELECT * FROM main_col WHERE col_link=".$rlink[$row]->table." AND col_type=1","col_id");
		if(empty($cls)) return $res;
		$c=getall3($db,"SELECT * FROM row_value WHERE value_value=$row AND value_col IN (".implode(',',$cls).")","value_row");
	}
	foreach($rows AS $rs){
		if(!empty($rs->sub)) $tmp=seek_linked($rs->sub,$row,$step+1,$c);
		else $tmp=Array();
		if(!empty($c[$rs->id]) || !empty($tmp)){
			$res[$i]=$rs;
			$res[$i]->sub=$tmp;
			$i++;
		}
	}
	return $res;
}

function seek_rowsub($row_id,$table_id){
	global $db,$rlink;
	seek_rlink($row_id);
	if(!empty($rlink[$row_id])){
		if($table_id!=$rlink[$row_id]->ownert) return $row_id;
		else if($rlink[$row_id]->owner==0) return 0;
		else return seek_rowsub($rlink[$row_id]->owner,$table_id);
	}
	
	/*stf('seek_rowsub');	
	getrow($db,"SELECT * FROM row_owner WHERE row_id=$row_id",1,'row_owner',__LINE__,__FILE__);	
	etf('seek_rowsub');
	if(!empty($db->Record)){
		if($table_id!=$db->Record["owner_table"]) return $row_id;//$db->Record["owner_id"];	//я не уверен в этой замене
		else if($db->Record["owner_id"]==0) return 0;
		else return seek_rowsub($db->Record["owner_id"],$table_id);
	}*/
}

function tree_optimize($tre,&$tree,$cols=Array()){//$tre - элемент цикла (нужен только для проверки есть ли у него id) (row), $tree - все элементы цикла
	global $tree_vars, $tv_cache,$vcache,$vcache2;
	//if(empty($cols)) return false;
	$type=1;
	if(empty($cols)){
		$ctree=$tree_vars[$GLOBALS["spec_step2"]];
	} else {
		$ctree=$cols;
		$type=2;
	}
	$res=false;
	if(!empty($ctree) && ((is_object($tre) && isset($tre->id)) || !empty($cols))){
		$res=true;//перенёс сюда, т.к. иногда при первом проходе цикла не проявляются столбцы
		
		//экспресс проверка на наличие свойств у объектов (проверяются три случайных объекта)
		$ctree2=Array();
		foreach($ctree AS $col){
			$xtrid=0;
			$bool=true;
			while($xtrid<=3){
				$trid_obj=$tree[array_rand($tree)];
				if(isset($trid_obj->id)){
					$trid=$trid_obj->id;			
					//echo $trid.' - '.$col["col_id"].'<br>';
					if(!isset($vcache[$col["col_id"]][$trid])){
						$bool=false;
						break;
					}
				}
				$xtrid++;
			}
			//echo $xtrid.'<br>';
			if(!$bool) $ctree2[]=$col;
		}
		$ctree=$ctree2;
		
		$tcol='';
		foreach($ctree AS $col){
			$tcol2[]=$col["col_id"];
			if($tcol=='') $tcol=$col["col_id"]; else $tcol.=','.$col["col_id"];
			$scol[$col["col_id"]]=$col;
		}
		$first2=true;
		$trow='';
		foreach($tree AS $var2=>$tre2){
			if(is_object($tre2) && isset($tre2->id)){
				if(!$first2 || $type==2){
					$trow2[]=$tre2->id;
					if($trow=='') $trow=$tre2->id; else $trow.=','.$tre2->id;
				}
				$first2=false;
			}
		}
		if($tcol!='' && $trow!=''){ sort($trow2);sort($tcol2);$sst=implode(',',$trow2).'|'.implode(',',$tcol2); }
		//echo 'tcol = '.$tcol.'<br>trow = '.$trow;
		if($tcol!='' && $trow!='') if(!isset($tv_cache[$sst])) {
			$tv_cache[$sst]=1;
			global $db;
			$vals=getall($db,"SELECT * FROM row_value WHERE value_row IN ($trow) AND value_col IN ($tcol)",1,"row_value",__LINE__,__FILE__);
			//echo "SELECT * FROM row_value WHERE value_row IN ($trow) AND value_col IN ($tcol)";
			$uc=Array(); global $ucache;
			if(!empty($vals)) foreach($vals AS $val){
				$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
				$vcache2[$val["value_col"]][$val["value_row"]]=$val;
				if(!empty($scol[$val["value_col"]]) && $scol[$val["value_col"]]["col_type"]==5 && empty($ucache[$val["value_value"]]) && !empty($val["value_value"])) $uc[$val["value_value"]]=$val["value_value"];
			}
			if(!empty($uc)){
				$tmp=implode(',',$uc);
				if(!empty($tmp)){
					$usr=getall($db,"SELECT * FROM main_auth WHERE auth_id IN (".implode(',',$uc).")",1,"main_auth");
					foreach($usr AS $u) $ucache[$u["auth_id"]]=$u;
				}
			}
			foreach($tcol2 AS $tcl) foreach($trow2 AS $trw){
				if(!isset($vcache[$tcl][$trw])) $vcache[$tcl][$trw]=Array();
				if(!isset($vcache2[$tcl][$trw])) $vcache2[$tcl][$trw]=Array();
			}
			global $no_cache;
			if($no_cache){unset($vcache); unset($vcache2);}
		}
	}
	return $res;
}

function e5log($title,$body){
	if(!file_exists(DOCUMENT_ROOT.'/core/log')){
		mkdir(DOCUMENT_ROOT.'/core/log');
		chmod(DOCUMENT_ROOT.'/core/log',DEF_DRMOD);
	}
	$date=date('Y-m-d H.i.s');
	$filename=DOCUMENT_ROOT.'/core/log/'.$title.' '.$date.'.log';
	$f=fopen($filename,'w+');
	fwrite($f,$body);
	fclose($f);
}

function do_log($text){
	/*global $debug,$log1;
	if($debug){
		if(empty($log1)) $log1=Array();
		$i=count($log1);
		$log1[$i]->t=$text;
		$trace=debug_backtrace();
		$i2=1;
		$log1[$i]->t.=' <span style="color: #AAAAAA;">('.basename($trace[$i2]["file"]).' '.$trace[$i2]["line"].')</span>';
		$log1[$i]->q=0;
	}*/
}

function get_tmp_tree($arr,$res=Array(),$prev=0){
	foreach($arr AS $a){
		if(is_object($a) && isset($a->id)) $i=$a->id; else $i=$a;
		if(is_array($a)) continue;
		$res[$i]->next=0;
		if(isset($res[$prev])) $res[$prev]->next=$i;
		$res[$i]->prev=$prev;
		if(is_object($a) && !empty($a->sub)){
			$res=get_tmp_tree($a->sub,$res,$i);
			$b=end($a->sub);
			if(is_object($b) && isset($b->id)) $c=$b->id; else $c=$b;
			$prev=$c;
		} else $prev=$i;
	}
	return $res;
}

function add_news($title,$url,$to,$row=0){
	global $user,$db,$cur_module,$cur_ex,$cur_part,$fullurl2;
	if(empty($cur_part)) $cur_part=0;
	$title=prepend_value(parse_var($title));
	$url=parse_var($url);
	if(!is_numeric($to)){
		$to=parse_var($to);
		if(!is_numeric($to)){
			$gi=getrowval("SELECT * FROM main_auth WHERE auth_type=1 AND (group_name='$to' OR group_sname='$to')","auth_id");
			if($gi!==false) $to=$gi; else $to=0;
		}
	}
	if(!empty($row) && empty($id)){
		$url=get_admin_url($row);
		if($url!='') $url.='#editform';
	}
	$fullurl3=$fullurl2;
	if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1) $fullurl3='%admin%';
	$fullurl3=prepend_value($fullurl3);
	$url=prepend_value($url);
	$db->query("INSERT INTO main_news (news_title, news_url, news_group, news_from, news_fromip, news_fromurl, news_read, news_row, news_module, news_part, news_ex, news_datetime, news_day)
	VALUES ('$title','$url',$to,$user->id,'$user->ip','$fullurl3',0,$row,$cur_module,$cur_part,".normalize_cur_ex($cur_ex).",'".date('Y-m-d H:i:s')."',".get_day(date('Y-m-d')).")",3,"main_news");
}

function get_news($limitN=0,$limitD=0,$moduleN=-1,$groupN=-2,$userFrom=-2,$show_viewed=0/* 0 - only new, 1 - only old all, 2 - only old viewed by current user, 3 - new and viewed by current user, 4 - all*/,$current=0,$except=Array()){
	global $user,$db,$rlink,$news_assoc;
	$news_assoc=Array();
	$day=get_day(date('Y-m-d'));
	$sql_m='';
	if($moduleN!=-1) $sql_m=' AND news_module='.$moduleN;
	if($groupN!=-2) $group=$groupN; else {
		$group=$user->id;
		if(!empty($user->group)) foreach($user->group AS $g) $group.=','.$g;
	}
	if($userFrom!=-2){
		$sql_g=' news_read='.$userFrom;
	} else {
		$sql_g=' news_group IN ('.$group.')';
		if($user->super) $sql_g='1=1';
	}
	if($show_viewed==4) $sql_w='';
	else if($show_viewed==0) $sql_w=' AND news_read=0';
	else if($show_viewed==1) $sql_w=' AND news_read!=0';
	else if($show_viewed==2) $sql_w=' AND news_read='.$user->id;
	else if($show_viewed==3) $sql_w=' AND (news_read='.$user->id.' OR news_read=0';
	if(!empty($except)) $sql_e=' AND !news_id IN ('.implode(',',$except).')'; else $sql_e='';
	if(empty($current)) $sql_c='';
	else $sql_c=' AND news_id='.$current;
	$sql="SELECT * FROM main_news WHERE ".$sql_g.$sql_m.$sql_w.$sql_c.$sql_e." ORDER BY news_datetime DESC";
	//echo $sql;
	$news2=getall($db,$sql,1,"main_news");
	$news=Array();
	if(!empty($news2)) foreach($news2 AS $n){
		$can_view=false;
		$can_edit=false;
		if($n["news_row"]!=0){
			$row=$n["news_row"];
			seek_rlink($row);
			if(empty($rlink[$row]) || empty($rlink[$row]->id)){
				$n["news_row"]=0;
			} else {
				$tr=get_top_row($row);
				$r=$rlink[$row];
				$module=$tr->module;
				$ex=get_ex2($tr->tex);
				$table=$r->table;
				$module=$tr->module;
				if(check_mod($module,'view') && check_ex($ex,'view') && check_tbl($table,'view') && check_row($row,$table,$ex,'view',$r->user,$r->users)){
					$can_view=true;
					if(check_row($row,$table,$ex,'edit',$r->user,$r->users)) $can_edit=true;
				}
			}
		}
		if($n["news_row"]==0){
			$module=$n["news_module"];
			$ex=$n["news_ex"];
			if(check_mod($module,'view') && check_ex($ex,'view')){
				$can_view=true;
				$can_edit=true;
			}
		}
		if($can_view){
			$ci=count($news);
			$d=explode(' ',$n["news_datetime"]);
			$d[1]=explode(':',$d[1]);
			$d[1]=$d[1][0].':'.$d[1][1];
			$news[$ci]->date=$d[0];
			$news[$ci]->time=$d[1];
			$news[$ci]->title=$n["news_title"];
			if($can_edit) $news[$ci]->url=$n["news_url"]; else $news[$ci]->url='';
			$news[$ci]->module=$n["news_module"];
			$news[$ci]->id=$n["news_id"];
			$news[$ci]->from_url=$n["news_fromurl"];
			$news[$ci]->group=$n["news_group"];
			$news[$ci]->module_name=get_module_name($n["news_module"]);			
			$news[$ci]->ex_name=getrowval("SELECT * FROM ex_module WHERE ex_id=$ex","ex_name");
			$news[$ci]->user='Инкогнито';
			if($n["news_from"]==0) $news[$ci]->user='Гость (ip='.$n["news_fromip"].')';
			else if($n["news_from"]==-1) $news[$ci]->user='Суперпользователь';
			else if(check_user(-$n["news_from"],'view')) $news[$ci]->user=getrowval("SELECT * FROM main_auth WHERE auth_id=".$n["news_from"]." AND auth_type=0","user_name").' ('.getrowval("SELECT * FROM main_auth WHERE auth_id=".$n["news_from"]." AND auth_type=0","user_login").')';
			$news_assoc[$n["news_id"]]=$ci;
		}
		if(($limitN!=0 && count($news)>=$limitN) || ($limitD!=0 && ($day-$n["news_day"])>=$limitD)) break;
	}
	if($current==0) return $news; else if(isset($news[0])) return $news[0]; else return false;
}

function fire($type=0,$error='',$halt=true){
	global $log_msgs;
	$text='Неизвестная ошибка';
	$img='fire';
	$subtext='';
	if($type=='db-error'){
		$text='Ошибка соединения с базой данных';
		$subtext='попробуйте <a href="">обновить страницу</a> через некоторое время или обратиться в службу технической поддержки';
		$img='fire';
	}
	
	if($type=='server-overload'){
		global $overload_value;
		$text='Перегрузка веб-сервера';
		$subtext='Текущее значение перегрузки - '.$error;
		$img='fire';
	}
	
	$traces=debug_backtrace();
	krsort($traces);
	$tr='';
	foreach($traces AS $trace) $tr.=str_replace(DOCUMENT_ROOT,'',$trace["file"]).' - '.$trace["line"].'<br>';	
	
	if($halt){
		@header('Content-type: text/html; charset="utf-8"');	
		echo '<html><head><title>Ошибка в работе</title><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><style>td,p,div{font-size: 12px;}</style><body style="font-family: Arial; font-size: 12px; background-color: #333333;"><div align="center"><div style="background-color: #FFFFFF; padding: 20px; margin: 50px; width: 500px; border: 4px solid #AAAAAA;" align="center"><table><tr><td valign="middle"><img src="'.$GLOBALS["base_root"].'/files/editor/'.$img.'.png" style="margin: 20px; margin-left: 0px;"></td><td valign="middle" align="center"><b style="font-size: 16px;">'.$text.'</b><br><br>'.$subtext.'<br><span style="color: #DDDDDD; font-size: 11px;">'.$error.'</span>';
		echo ' <span style="color: #888888; font-size: 20px; font-weight: bold; cursor: pointer;" OnClick="document.getElementById(\'wayback\').style.display=\'\';">+</span>';
		echo '<div style="display: none; font-size: 11px; color: #9E9E9E;" id="wayback">';
		echo $tr;
		if(!empty($log_msgs)) echo '<br><div align="center">= = = LOG = = =</div>'.implode('<br>',$log_msgs);
		echo '</div>';
		echo '</td></tr></table></div></div></body></html>';
	}
	
	global $send_error_reports, $last_send_report, $system_email;
	if($send_error_reports!=''){
		$xtime=date_to_xhour()*60+date('i');
		if($xtime-5>=$last_send_report || !$last_send_report){
			$last_send_report=$xtime;
			save_timing();
			$get=collect_text_data($_GET,' = ','<br>');
			$post=collect_text_data($_POST,' = ','<br>');
			$server=collect_text_data($_SERVER,' = ','<br>');
			
			$mail_body='
Текст:<br>
'.$text.'
<br><br>
Подробнее:<br>
'.$subtext.'
<br><br>
Ошибка:<br>
'.$error.'
<br><br>
Трейс:<br>
'.$tr.'
<br><br><br>

GET переменные:<br>
'.$get.'
<br><br><br>

POST переменные:<br>
'.$post.'
<br><br><br>

SERVER переменные:<br>
'.$server.'
<br><br><br>

Время на сервере: '.date('Y-m-d H:i:s');

			simple_mail($system_email,$send_error_reports,'Ошибка на сайте '.$_SERVER["HTTP_HOST2"],$mail_body);
		}
	}
	
	if($halt) die();
}

function collect_text_data($arr,$sep1='',$sep2=''){
	$res='';
	if(!empty($arr)) foreach($arr AS $var=>$value){
		if(!empty($res)) $res.=$sep2;
		$res.=$var.$sep1.$value;
	}
	return $res;
}

function start_safe_output(){
	if(empty($GLOBALS["zone_deny"]) || $GLOBALS["zone_deny"]!=$GLOBALS["e5uid"]){
		$GLOBALS["ssf_out_tmp"]=ob_get_contents();
		if($GLOBALS["ssf_out_tmp"]) ob_end_clean();
		ob_start();
	}
}

function stop_safe_output(){
	if((empty($GLOBALS["zone_deny"]) || $GLOBALS["zone_deny"]!=$GLOBALS["e5uid"]) && !empty($GLOBALS["xn_user_safe"])){
		$data=ob_get_contents();
		ob_end_clean();
		if($GLOBALS["ssf_out_tmp"]){
			ob_start();
			echo $GLOBALS["ssf_out_tmp"];
		}
		$dt='<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">';
		if(substr($data,0,strlen($dt))==$dt){ echo $dt; $data=substr($data,strlen($dt));}
		if(!empty($GLOBALS["sp_head"])){ echo $GLOBALS["sp_head"]; $data=str_replace($GLOBALS["sp_head"],'',$data);}
		$data=simple_string(soft_code2($data, md5($GLOBALS["xn_user_safe"].md5($GLOBALS["xn_user_safe"].$GLOBALS["xn_user_protect"]))));
		$data=htmlspecialchars($data,ENT_QUOTES);
		echo '<textarea id="cnt" style="display: none;">'.$data.'</textarea><script> var x=document.getElementById("cnt").value; x=utf8_decode(soft_decode2(desimple_string(x),md5($.jStorage.get("z-hidden")+getCookie("z-auth")))); document.write(x);</script>';		
	}
}

function prepend_file($filename){
	//функция для защиты файла при прямом присваивании в БД, чтобы нельзя было присвоить таким образом файл конфига
	if(strstr($filename,'config.inc') || strstr(strtolower($filename),'config.inc')) return '';
	return $filename;
}

function proc_log(){
	$filename=FTEMP.'proc.log';
	$sep='	';
	$sep2=chr(10);

	$proc_log_max_size=0.8;//в мб
	check_dir(FTEMP);
	if(!file_exists($filename)){
		$f=fopen($filename,'a');
		$filesize=0;
	} else {
		$f=fopen($filename,'a');
		$filesize=filesize($filename);
	}
	//format:	PID	IP	DOMAIN	URL		TIME_STARTED
	$line=Array();
	$line[]=getmypid();
	if(!empty($_SERVER["REMOTE_ADDR"])) $line[]=$_SERVER["REMOTE_ADDR"];
	else $line[]='Uncknown IP';
	if(!empty($_SERVER["SERVER_NAME"])) $line[]=$_SERVER["SERVER_NAME"];
	else $line[]=$_SERVER["HTTP_HOST"];
	$line[]=$_SERVER["REQUEST_URI"];	
	$line[]=date('Y-m-d H:i:s');
	$line[]=date('H:i');
	$line=$sep2.implode($sep,$line);
	fwrite($f,$line);
	fclose($f);
	if($filesize>$proc_log_max_size*1024*1024){
		//отрезаем на треть с конца
		$x=file_get_contents($filename);
		$x=substr($x,$filesize-floor(($proc_log_max_size/3)*1024*1024));
		$f=fopen($filename,'w');
		fwrite($f,$x);
		fclose($f);
	}
}

function extract_from_log($some,$time=''){
	$filename=FTEMP.'proc.log';
	$sep='	';
	$sep2=chr(10);
	
	global $pl_temp_pid,$pl_temp_ip,$pl_temp_domain,$pl_temp_pid2;
	$filename=FTEMP.'proc.log';
	if(!file_exists($filename)) return false;
	if(is_numeric($some)) $type='pid';
	else if(strstr($some,'.')){
		$t=explode('.',$some);
		if(count($t)==4 && is_numeric($t[0]) && is_numeric($t[3])) $type='ip';
		else $type='domain';
	}
	$ch=(int)date('H');
	if($ch>1) $ch2=$ch-1; else $ch2=24;
	if(empty($pl_temp_pid)){
		$pl_temp=file_get_contents($filename);
		$pl_temp=explode($sep2,$pl_temp);
		if(!empty($pl_temp)) foreach($pl_temp AS $line){
			$line=explode($sep,$line);
			if(empty($line) || count($line)<6) continue;
			//format:	PID	IP	DOMAIN	URL		TIME_STARTED
			$o=Array();
			$o["id"]=$line[0];
			$o["ip"]=$line[1];
			$o["domain"]=$line[2];
			$o["url"]=$line[3];
			$o["time"]=$line[4];
			$o["time2"]=$line[5];
			$h=explode(':',$line[5]);
			$h=(int)$h[0];
			$pl_temp_pid[$o["id"]]=$o;
			if($h==$ch || $h==$ch2) $pl_temp_pid2[$o["id"]][$o["time2"]]=$o;
			$pl_temp_ip[$o["ip"]][]=&$pl_temp_pid[$o["id"]];
			$pl_temp_domain[$o["domain"]][]=&$pl_temp_pid[$o["id"]];
		}
	}
	if($type=='pid' && empty($time) && isset($pl_temp_pid[$some])) return $pl_temp_pid[$some];
	if($type=='pid' && !empty($time) && isset($pl_temp_pid2[$some]) && isset($pl_temp_pid2[$some][$time])) return $pl_temp_pid2[$some][$time];
	if($type=='ip' && isset($pl_temp_ip[$some])) return $pl_temp_ip[$some];
	if($type=='domain' && isset($pl_temp_domain[$some])) return $pl_temp_domain[$some];
	return false;
}

function concat_with_zone($rurl,$rurl2){
	global $zone_folder;
	if(empty($zone_folder)) return $rurl.$rurl2;
	else {
		if(strlen($rurl2)>strlen($zone_folder) && substr($rurl2,0,strlen($zone_folder)+1)=='/'.$zone_folder) $rurl2=substr($rurl2,strlen($zone_folder)+1);
		if($rurl[strlen($rurl)-1]=='/' && $rurl2[0]=='/') $rurl2=substr($rurl2,1);
		if($rurl[strlen($rurl)-1]!='/' && $rurl2[0]!='/') $rurl2='/'.$rurl2;
		return $rurl.$rurl2;
	}
}

function show_form($col,$table,$row=0,$force_form=0,$value=''){
	global $db;
	$r=backup_globals();
	$GLOBALS["cur_table"]=$table;
	$GLOBALS["cur_row"]=$row;
	//$GLOBALS["cur_col"]=$col["col_sname"];
	unset($GLOBALS["cur_col"]);
	$GLOBALS["cur_col"]->id=$col["col_id"];
	$GLOBALS["cur_col"]->row=$row;
	$GLOBALS["ex_ex2"]=$GLOBALS["cur_ex"];
	$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
	if(empty($GLOBALS["p_par"])) $GLOBALS["p_par"]='';
	$pt=$GLOBALS["p_par"];
	$cex=$GLOBALS["cur_ex"];
	if(strstr($cex,'?')){
		$cex=explode('?',$cex);
		$cex=$cex[0];
	}
	if(!empty($cex)){
		getrow($db,"SELECT * FROM ex_group WHERE ex_table=$table AND ex_ex2=".$cex,2,"ex_group",__LINE__,__FILE__);
		$tex=$db->Record["ex_ex1"];
	} else $tex=0;
	$defs=Array();
	if($value!='') $defs[$col["col_id"]]=$value;
	if(!isset($GLOBALS["glob_prefix"])) $GLOBALS["glob_prefix"]='';
	$res=input_form($table,$col["col_module"],$row,$col["col_id"],$tex,$cex,$force_form,0,$defs,$GLOBALS["glob_prefix"]);
	return_globals($r);
	$GLOBALS["p_par"]=$pt;
	return $res;
}

?>