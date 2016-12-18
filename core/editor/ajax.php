<?php

if(!empty($left_url)){
	$i_url=DOCUMENT_ROOT.'/core/editor/ajax/'.implode('/',$left_url).'.php';
	if(file_exists($i_url)) include($i_url);
	exit;
}

if(empty($action)) $action='';
if(isset($_POST["action"])) $action=$_POST["action"];
if(isset($_POST["x"])) $x=urldecode($_POST["x"]);
if(isset($_POST["y"])) $y=urldecode($_POST["y"]);
if(isset($_POST["z"])) $z=urldecode($_POST["z"]);
if(isset($_POST["a"])) $a=urldecode($_POST["a"]);
if(isset($_POST["b"])) $b=urldecode($_POST["b"]);
if(isset($_POST["c"])) $c=urldecode($_POST["c"]);
if($action=='add_ex_form'){
	if(!empty($y)) add_ex_form($x,0,$y);
	else add_ex_form($x,0);
}
if($action=='ex_overload_form'){
	$x=explode('!',$x);
	$table_id=$x[0];
	$module_id=$x[1];
	$row_id=$x[2];
	$ex_ex1=$x[3];
	$ex_ex2=$x[4];
	echo input_form($table_id,$module_id,$row_id,0,$ex_ex1,$ex_ex2,0,0,Array(),'',1,1,1);
}
if($action=='add_row_form'){
	$x=explode('!',$x);
	$prefix=$x[0];
	$table_id=$x[1];
	$module_id=$x[2];
	$tex_id=$x[3];
	$ex_id=$x[4];	
	$skip_ex=$x[5];
	$id6=$x[6];
	$GLOBALS["cur_table"]=$table_id;
	$GLOBALS["cur_ex"]=$ex_id;
	$GLOBALS["ex_ex2"]=$GLOBALS["cur_ex"];
	if(check_operation('add',0,$id6,$ex_id,$table_id)){
		echo '<input type="hidden" name="'.$prefix.'info" value="'.implode('!',$x).'">';
		echo input_form($table_id,$module_id,0,0,$tex_id,$ex_id,0,$skip_ex,Array(),$prefix);
	}
}

if($action=='get_sql_data'){
	global $user;
	if($user->super){
		$f=DOCUMENT_ROOT.'/core/units/backup/'.$x;
		header('Content-Type: application/octet-stream'); 
		header ("Accept-Ranges: bytes");
		header ("Content-Length: ".filesize($f));
		header ("Content-Disposition: attachment; filename=".end(explode('/',$f)));
		readfile($f);
		exit;
	}
}

if($action=='check_unique'){
	global $db;
	if($z>0){
		$rws=getall($db,"SELECT * FROM row_owner WHERE row_table=$x AND owner_id=$y AND ro_ex=$z",1,"row_owner");
	} else {
		$rws=getall($db,"SELECT * FROM row_owner WHERE row_table=$x AND ro_ex=".(-$z),1,"row_owner");
	}
	$spl=Array();
	foreach($rws AS $rw) $spl[$rw["row_id"]]=$rw["row_id"];
	if(!empty($spl)){
		getrow($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$spl).") AND value_col=$a AND value_value='$b' AND value_row!=$c",1,"row_value");
		if(!empty($db->Record)) echo '1';
	}
}

//save tpl in row
if($action=='save_part' && empty($part) && !empty($col) && !empty($row) && $row>0){
	global $user, $rlink,$db;
	$col=safe_sql_input($col);
	$row=safe_sql_input($row);
	//$body=safe_sql_input($body);
	seek_rlink($row);
	if(empty($rlink[$row])) exit;
	$r=$rlink[$row];
	if(!check_row($row,$r->table,get_ex2($r->tex),'edit',$r->user,$r->users)) exit;
	$table=$r->table;
	$module=$r->module;
	$c=getrow($db,"SELECT * FROM main_col WHERE col_id=$col");
	if(empty($c)) exit;
	$value_id=getrowval("SELECT value_id FROM row_value WHERE value_row=$row AND value_table=$table AND value_col=$col","value_id");
	if(empty($value_id)){
		$db->query("INSERT INTO row_value (value_row, value_table, value_module, value_col, value_value)
					VALUES ($row, $table, $module, $col, '$body')");
	} else {
		$db->query("UPDATE row_value SET value_value='$body' WHERE value_row=$row AND value_table=$table AND value_col=$col AND value_module=$module");
	}

	$col_tpl=$c['col_tpl'];
	$col_uin=$c['col_uin'];
	$table_id=$c['col_table'];
	$module_id=$c['col_module'];
	if($col_tpl) update_part_links($row,'val',$col_uin,$body,'row',$table_id,$module_id);
	echo '1';
	exit;
}

//save tpl in ex
if($action=='save_part' && empty($part) && !empty($col) && !empty($row) && $row<0){
	global $user;
	$col=safe_sql_input($col);
	$row=-$row;
	$ex=safe_sql_input($row);
	//$body=safe_sql_input($body);
	$module=getrowval("SELECT ex_module FROM ex_module WHERE ex_id=$ex","ex_module");
	if(!$module) exit;
	if(!check_ex($ex,'edit')) exit;
	$c=getrow($db,"SELECT * FROM main_col WHERE col_id=$col");
	if(empty($c)) exit;
	$table=0;
	$value_id=getrowval("SELECT value_id FROM row_value WHERE value_row=$row AND value_table=$table AND value_col=$col","value_id");
	if(empty($value_id)){
		$db->query("INSERT INTO row_value (value_row, value_table, value_module, value_col, value_value)
					VALUES ($row, $table, $module, $col, '$body')");
	} else {
		$db->query("UPDATE row_value SET value_value='$body' WHERE value_row=$row AND value_table=$table AND value_col=$col AND value_module=$module");
	}
	
	$col_tpl=$col['col_tpl'];
	$col_uin=$col['col_uin'];
	$table_id=$col['col_table'];
	$module_id=$col['col_module'];
	if($col_tpl) update_part_links($ex,'val',$col_uin,$body,'row',$table_id,$module_id);
	echo '1';
	exit;
}

//save part
if($action=='save_part' && !empty($part) && empty($row)){
	global $user;
	getrow($db,"SELECT * FROM main_part WHERE part_id=".$part);
	foreach($db->Record AS $var=>$value)$$var=$value;
	if($part_type==2 && $user->super==0) exit; //если это компонент и юзер не суперпользователь - выходим
	if(!check_prt($part_id,'edit')) exit; //если нет доступа для правки этой части - выходим
	if($part_type!=2 && !empty($part_module) && !check_mod($part_module,'edit')) exit; //если это часть модуля и нет доступа к этому модулю - выходим
	if($part_type!=2 && !empty($part_module)) update_module_state($part_module);
	del_cache('part',$part_id);
	update_part_state($part_id);
	if(empty($part_uin)){
		$part_uin=uuin();
		$db->query("UPDATE main_part SET part_uin='$part_uin' WHERE part_id=$part",3,"main_part");
	}
	if($part_type!=2){
		update_part_links($part_module,'part',$part_uin,$body);
	} else {
		update_part_links($part_id,'part',$part_uin,$part_body,'part');
	}
	$db->query("UPDATE main_part SET
		part_body='$body'
	WHERE part_id=$part_id",3,'main_part');
	echo '1';
	exit;
}

if($action=='load_users'){
	global $user;
	$sel_user=select_users(0,($user->super?1:0),1);
	echo $sel_user;
}

function clear_null(&$data){
	foreach($data AS $k=>$v){
		if(is_array($v)) clear_null($data[$k]); else if(is_null($v)) $data[$k]='';
	}
}

if($action=='get_im_json'){
	global $db;
	if(!file_exists(JTEMP.$name.'.json')){
		$tmp=Array();
		if($name=='cmd' || $name=='component' || $name=='widget'){
			if($name=='cmd'){
				foreach($GLOBALS['cmd'] AS $var=>$value){
					foreach($value AS $var2=>$value2){
						if(isset($value2->visual)){
							$x=$value2;
							while(isset($x->alias)) $x=$x->alias;
							if(isset($value2->visual['pos'])) $pos=$value2->visual['pos'];
							else if(isset($x->visual['pos'])) $pos=$x->visual['pos']; else $pos=1000;
							if(isset($value2->visual['group'])){
								$g=$value2->visual['group'];
								$ind=$var2+10000+$g;
								$tmp[$ind][$pos][$var]=$value2->visual;
								if(isset($x->result)) $tmp[$ind][$pos][$var]['result']=$x->result;
								else $tmp[$ind][$pos][$var]['result']=CMD_NONE;
								if(isset($value2->result)) $tmp[$ind][$pos][$var]['result']=$value2->result; //NEW LINE ACHTUNG!!!
								if(isset($value2->visual['result'])) $tmp[$ind][$pos][$var]['result']=$value2->visual['result'];
								if(isset($value2->result_long)) $tmp[$ind][$pos][$var]['result_long']=$value2->result_long;
								$tmp[$ind][$pos][$var]['cmd']=$var;
								$tmp[$ind][$pos][$var]['cmd_type']=$var2;
								$gpos=1000;
								if(!isset(/*$tmp[$var2+$g]*/$tmp[$var2][$gpos][$g])){
									$cg=$GLOBALS['cmd_group'][$g];
									if(isset($cg['pos'])) $gpos=$cg['pos'];
									$tmp[$var2][$gpos][$g]=$cg;
									$tmp[$var2][$gpos][$g]['result']=$ind;
									$tmp[$var2][$gpos][$g]['disable_insert']=1;
									$tmp[$var2][$gpos][$g]['is_group']=1;
									$tmp[$var2][$gpos][$g]['skip_folder']=1;
									//if(isset($cg['work_on'])) $tmp['work_on'][$cg['work_on']][$gpos][]=$tmp[$var2][$gpos][$g];
								}
								if(isset($value2->visual['work_on'])){
									$tmp['work_on'][$value2->visual['work_on']][$pos][$var]=$tmp[$ind][$pos][$var];//$tmp[$var2][$pos][$var];
								}
							} else {
								$tmp[$var2][$pos][$var]=$value2->visual;
								if(isset($x->result)) $tmp[$var2][$pos][$var]['result']=$x->result;
								else $tmp[$var2][$pos][$var]['result']=CMD_NONE;
								if(isset($value2->result)) $tmp[$ind][$pos][$var]['result']=$value2->result; //NEW LINE ACHTUNG!!!
								if(isset($value2->visual['result'])) $tmp[$var2][$pos][$var]['result']=$value2->visual['result'];
								if(isset($value2->result_long)) $tmp[$var2][$pos][$var]['result_long']=$value2->result_long;
								$tmp[$var2][$pos][$var]['cmd']=$var;
								$tmp[$var2][$pos][$var]['cmd_type']=$var2;
								if(isset($value2->visual['work_on'])) $tmp['work_on'][$value2->visual['work_on']][$pos][$var]=$tmp[$var2][$pos][$var];
							}
						}
					}
				}
				foreach($tmp AS $var=>$value)if($var!='work_on'){
					ksort($tmp[$var]);
				}
				foreach($tmp['work_on'] AS $var=>$value){
					ksort($tmp['work_on'][$var]);
				}
				/*foreach($tmp AS $var=>$value){
					echo '<h1>'.$var.'</h1>';
					foreach($value AS $var2=>$value2){
						foreach($value2 AS $var3=>$value3){
							echo '<div>'.$var2.': '.$var3.'</div>';
						}
					}
				}*/
			}
			if($name=='component'){
				$cas=getall($db,"SELECT * FROM part_cat");
				foreach($cas AS $ca){
					$tmp[$ca['cat_type']][$ca['cat_id']]['name']=$ca['cat_name'];
				}
				$cs=getall($db,"SELECT part_id, part_cat, part_name, part_sname, part_proc, part_major, part_module, part_about FROM main_part WHERE part_type=2");
				foreach($cs AS $c){
					$tmp[$c['part_proc']][$c['part_cat']]['parts'][$c['part_id']]=$c;
				}
				$ps=getall($db,"SELECT p.part_proc, p.part_cat, pp.param_part, pp.param_id, pp.param_name, pp.param_sname, pp.param_default, pp.param_array, pp.param_get, pp.param_type, pp.param_list, pp.param_link FROM part_param AS pp LEFT JOIN main_part AS p ON p.part_id=pp.param_part WHERE p.part_type=2 AND !pp.param_hide");
				foreach($ps AS $p){
					$tmp[$p['part_proc']][$p['part_cat']]['parts'][$p['param_part']]['params'][$p['param_id']]=$p;
				}
			}
			if($name=='widget'){
				$mods=getall($db,"SELECT module_id, module_sname, module_name FROM main_module");
				if(!empty($mods)) foreach($mods AS $mod) if(check_mod($mod['module_id'],'view')){
					$parts=getall4($db,"SELECT part_sname, part_name, part_module, part_id, part_about FROM main_part WHERE part_module=".$mod['module_id']." AND part_type=1","part_id");
					if(!empty($parts)){
						$exs=getall($db,"SELECT ex_name, ex_sname, ex_module, ex_id FROM ex_module WHERE ex_module=".$mod['module_id']);
						$aexs=Array();
						if(!empty($exs)) foreach($exs AS $ex) if(check_ex($ex['ex_id'],'view')){
							$aexs[$ex['ex_sname']]=$ex['ex_name'];
						}
						if(!empty($aexs)){
							$tmp[$mod['module_sname']]=$mod;
							$tmp[$mod['module_sname']]['parts']=$parts;
							$tmp[$mod['module_sname']]['exs']=$aexs;
							if(!empty($parts)) foreach($parts AS $part_id=>$p){
								$tmp[$mod['module_sname']]['parts'][$part_id]['params']=getall4($db,"SELECT param_part, param_id, param_name, param_sname, param_default, param_array, param_get, param_type, param_list, param_link FROM part_param WHERE param_part=$part_id AND !param_hide","param_id");
							}
						}
					}
				}
				clear_null($tmp);
				echo json_encode($tmp);
				exit;
			}
		} else {
			// MODULES
			if(!is_numeric($name)) $module_id=getrowval("SELECT module_id FROM main_module WHERE module_sname='".safe_sql_input($name)."'",'module_id');
			else $module_id=safe_sql_input($name);
			global $loaded_module;
			$loaded_module=Array();
			function load_module_ide($module_id){
				global $loaded_module,$db;
				$loaded_module[$module_id]=1;
				$tmp=Array();
				$m=getrow($db,"SELECT module_id, module_name, module_major, module_sname FROM main_module WHERE module_id=$module_id");
				$tmp['id']=$m['module_id'];
				$tmp['name']=$m['module_name'];
				//$tmp['major']=$m['module_major'];
				$tmp['sname']=$m['module_sname'];
				//tables
				$tmp['tables']=getall4($db,"SELECT table_id, table_name, table_sname, table_multy, table_bold, major_col FROM main_table WHERE table_module=$module_id",'table_id');			
				if(!empty($tmp['tables'])) foreach($tmp['tables'] AS $table_id=>&$t){
					if($t['table_bold']==1) $tmp['major']=$table_id;
					//subtables
					$t['subtables']=getall3($db,"SELECT sub_table2 FROM table_sub WHERE sub_table1=$table_id","sub_table2");
					if(!empty($t['subtables'])) foreach($t['subtables'] AS $st){
						$sm=getrowval("SELECT table_module FROM main_table WHERE table_id=$st",'table_module');
						if($sm!=$module_id){
							if(empty($loaded_module[$sm])){
								$tmp['foreign'][$sm]=load_module_ide($sm);
								if(!empty($tmp['foreign'][$sm]['foreign'])){
									foreach($tmp['foreign'][$sm]['foreign'] AS $mid=>$fm){
										if(empty($tmp['foreign'][$mid])){
											$tmp['foreign'][$mid]=$fm;
										}
									}
								}
							}
							$tmp['foreign_table'][$st]=$sm;
						}
					}
					//cols
					$t['cols']=getall4($db,"SELECT col_id, col_name, col_sname, col_hint, col_type, col_default, col_url, col_link, col_link2, col_deep, module_url, col_inform FROM main_col WHERE col_table=$table_id AND col_module=$module_id ORDER BY col_pos","col_id");
				}
				//ex params
				$tmp['params']=getall4($db,"SELECT col_id, col_name, col_sname, col_hint, col_type, col_default, col_url, col_link, col_link2, col_deep, module_url, col_inform FROM main_col WHERE col_table=0 AND col_module=$module_id ORDER BY col_pos","col_id");
				//exs
				$tmp['exs']=getall4($db,"SELECT ex_id, ex_name, ex_sname, ex_major FROM ex_module WHERE ex_module=$module_id","ex_id");
				//parts (с учётом вложенности, хотя лучше вложенность проверять по факту с помощью owner и id)
				$tmp['parts']=getall10($db,"SELECT part_id, part_name, part_sname, part_about, part_module, part_type, part_access, part_table, part_url, part_owner, part_404, part_major FROM main_part WHERE part_module=$module_id","part_owner","part_id");
				//parts params
				if(!empty($tmp['parts'])) foreach($tmp['parts'] AS $part_owner=>$parts) foreach($parts AS $part_id=>$p){
					$tmp['parts'][$part_owner][$part_id]['params']=getall4($db,"SELECT param_part, param_id, param_name, param_sname, param_default, param_array, param_get, param_type, param_list, param_link FROM part_param WHERE param_part=$part_id AND !param_hide","param_id");
				}
				//user groups
				$tmp['groups']=getall4($db,"SELECT auth_id, group_name, group_sname FROM main_auth WHERE auth_type=1 AND group_module=$module_id",'auth_id');
				return $tmp;
			}			
			$tmp=load_module_ide($module_id);
		}
		if(!empty($tmp)){
			//обрезаем NULL
			clear_null($tmp);
			//превращаем в JSON и сохраняем
			$d=json_encode($tmp);
			check_dir(JTEMP);
			$f=fopen(JTEMP.$name.'.json','w');
			fwrite($f,$d);
			fclose($f);
		}
	}
	if(file_exists(JTEMP.$name.'.json')){
		//здесь должна будет быть проверка check_module
		echo file_get_contents(JTEMP.$name.'.json');
	}
}

if($action=='get_ex_for_table' && !empty($table)){
	$table=safe_sql_input($table);
	$module=getrowval("SELECT table_module FROM main_table WHERE table_id=$table",'table_module');
	if(!$module) exit;
	$exes=getall6($db,"SELECT ex_module, ex_name, ex_id FROM ex_module WHERE ex_module=$module",'ex_id','ex_name');
	$tmp=Array();
	foreach($exes AS $ex_id=>$ex) if(check_ex($ex_id,'view')) $tmp[$ex_id]=$ex;
	echo json_encode($tmp);
	exit;
}

if($action=='get_rows_for_table' && !empty($table) && !empty($ex)){
	$table=safe_sql_input($table);
	$ex=safe_sql_input($ex);
	$module=getrowval("SELECT table_module FROM main_table WHERE table_id=$table",'table_module');
	//$table_ex=get_tex(0,$ex,$table);
	$rows=get_simple_options($table,$ex,0,2,1);//get_sub(0,$table,1,1,0,0,0,$table_ex,$ex,$table,0,0,1);
	echo json_encode($rows);
	exit;
}

if($action=='get_remote_rows' && !empty($t) && !empty($m)){
	global $db;
	$t=getrowval("SELECT table_uin FROM main_table WHERE table_id=$t","table_uin");
	$m=getrowval("SELECT module_uin FROM main_module WHERE module_id=$m","module_uin");
	$exs=unserialize(loadserv($GLOBALS["update_server"].'?type=show_rows_exs&table_id='.$t.'&module_id='.$m));
	//echo $GLOBALS["update_server"].'?type=show_rows_exs&table_id='.$t.'&module_id='.$m.'<br>';		
	$tmp=explode('///',loadserv($GLOBALS["update_server"].'?type=part_list'));
	$parts_src=Array();
	if(!empty($tmp)) foreach($tmp AS $t){
		$t=explode('|||',$t);
		if(!empty($t[2]) && $t[2]!='links'){
			$parts_src[$t[2]]->id=$t[4];
			$parts_src[$t[2]]->name=$t[1];
		}
	}
	$tmp=getall($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2",1,"main_part");
	$parts_dst=Array();
	if(!empty($tmp)) foreach($tmp AS $t){
		$parts_dst[$t['part_proc']][$t['part_sname']]=1;
	}
	if(!empty($exs)){
		foreach($exs AS $ex){
			echo '<h2>'.$ex['ex_name'].'</h2>';
			seek_row_part_links($ex['rows'],$parts_src,$parts_dst);
			echo checkbox($ex['rows'],'','upload',0,Array(),0);
		}
	} else {
		echo '<h2>Нет доступных элементов</h2>';
	}
	exit;
}

if($action=='get_remote_ex' && !empty($m)){
	global $db;
	$m=getrowval("SELECT module_uin FROM main_module WHERE module_id=$m","module_uin");
	//echo $GLOBALS["update_server"].'?type=show_exs&module_id='.$m;
	$exs=unserialize(loadserv($GLOBALS["update_server"].'?type=show_exs&module_id='.$m));
	$tmp=explode('///',loadserv($GLOBALS["update_server"].'?type=part_list'));
	$parts_src=Array();
	if(!empty($tmp)) foreach($tmp AS $t){
		$t=explode('|||',$t);
		if(!empty($t[2]) && $t[2]!='links'){
			$parts_src[$t[2]]->id=$t[4];
			$parts_src[$t[2]]->name=$t[1];
		}
	}
	$tmp=getall($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2",1,"main_part");
	$parts_dst=Array();
	if(!empty($tmp)) foreach($tmp AS $t){
		$parts_dst[$t['part_proc']][$t['part_sname']]=1;
	}
	/*function seek_row_part_links(&$rows,$src,$dst){
		foreach($exs AS $index=>$ex){
			if(!empty($ex->components)){
				$r='';
				$ids=Array();
				foreach($ex->components AS $part_proc=>$tmp) foreach($tmp AS $part_sname=>$t) if(empty($dst[$part_proc][$part_sname])){
					//if(!empty($r)) $r.=', ';
					$r.='<br>- '.$src[$part_sname]->name;
					$ids[]=$src[$part_sname]->id;
				}
				if(!empty($r)){
					$rows[$index]->description='<div style="background-color: #EFEFEF; padding: 5px; margin-bottom: 5px;">';
					$rows[$index]->description.='<input type="hidden" name="install_components['.$ex->id.']" value="'.implode('^^',$ids).'">';
					$rows[$index]->description.='Автоматически будут загружены следующие компоненты:'.$r;
					$rows[$index]->description.='</div>';
				}
			}
		}
	}*/
	if(!empty($exs)){
		echo '<style>.upload_descr{padding: 5px; background-color: #FEFEFE;}</style>';
		seek_row_part_links($exs,$parts_src,$parts_dst);
		//echo checkbox($exs,'','upload',0,Array(),0);
		echo '<p>Экземпляр:<br><select name="ex_upload" OnChange="$(\'.upload_descr\').each(function(){$(this).hide()}); $(\'#ex_description\'+$(this).val()).show();">';
		echo options($exs,'',0,1,0,0,0,'',Array(),10000,0,0,0,0,0/*,'ex_description'*/);
		echo '</select></p>';
		$first=true;
		foreach($exs AS $ex){
			if($first){
				$add='';
				$first=false;
			} else $add=' style="display: none;"';
			if(!empty($ex->description)) echo '<div class="upload_descr" id="ex_description'.$ex->id.'"'.$add.'>'.$ex->description.'</div>';
		}
		echo '<div>Внимание! Перед загрузкой убедитесь, что модуль обновлён до актуального состояния</div>';
	} else {
		echo '<h2>Нет доступных элементов</h2>';
	}
	exit;
}

?>