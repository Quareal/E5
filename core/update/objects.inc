<?php

include_once('functions.inc');

$exp_rows=Array();
$exp_rows2=Array();
$rstart='{{!|';
$rend='|!}}';
$import_rows=0;
$import_links=0;
$collect_parts=0;
$collected_parts=Array();

function start_export($set_collect_parts=0,$set_export_started=true){//also import
	global $exp_rows,$import_rows,$import_links,$from_linked,$top_level,$collect_parts,$collected_parts,$is_export_started;
	$exp_rows=Array();
	$exp_rows2=Array();
	$exp_folders=Array();
	$import_rows=0;
	$import_links=0;
	if($set_collect_parts) $collect_parts=1;
	$collected_parts=Array();
	$recheck_rows=Array();
	if($set_export_started) $is_export_started=true;
}

function end_export(){//also import
	global $exp_rows,$import_rows,$import_links,$recheck_rows,$db,$collect_parts,$is_export_started;
	$exp_rows=Array();
	$exp_rows2=Array();
	//$collect_parts=0;
	//echo $import_rows.'<br>'.$import_links;
	// на всякий случай проверка
	if(!empty($recheck_rows)) foreach($recheck_rows AS $recheck){
		$lnk=explode('.',$recheck["link"]);
		$row_module=$lnk[0];
		$row_table=$lnk[1];
		$sid=$lnk[2];
		$c=$lnk[3];
		$uin=$recheck["uin"];
		$value=get_uin3('row',$uin);
		if(!empty($value)){
			$sql="INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
							VALUES ($row_module, $row_table, $sid, $c, '$value')";
			$db->query($sql,3,'row_value',__LINE__,__FILE__);		
		}
	}
	$recheck_rows=Array();
	$is_export_started=false;
}

function import_modex($module_uin, $ex_uin){
	$v_module=get_uin3('module',$module_uin);
	if(empty($v_module)){
		$v_module=complex_import_module($module_uin);
	}
	$v_ex=get_uin3('ex',$ex_uin,'ex_module',$v_module);
	if(empty($v_ex) && !empty($v_module)){
		$v_ex=import_ex(0,$ex_uin,false,$v_module);
	}
}

function get_uin2($from,$id){
	global $uin_cache,$db;
	if(isset($uin_cache[$from][$id])) return $uin_cache[$from][$id];
	if($from=='module') $uin_cache[$from][$id]=getrowval("SELECT module_id, module_uin FROM main_module WHERE module_id=$id","module_uin");
	if($from=='table') $uin_cache[$from][$id]=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_id=$id","table_uin");
	if($from=='part')  $uin_cache[$from][$id]=getrowval("SELECT part_id, part_uin FROM main_part WHERE part_id=$id","part_uin");
	if($from=='ex'){
		$uin_cache[$from][$id]=getrowval("SELECT ex_id, ex_uin FROM ex_module WHERE ex_id=$id","ex_uin");
		if(empty($uin_cache[$from][$id])){
			$uin_cache[$from][$id]=uuin();
			$db->query("UPDATE ex_module SET ex_uin='".$uin_cache[$from][$id]."' WHERE ex_id=$id");
		}
	}
	if(isset($uin_cache[$from][$id])) return $uin_cache[$from][$id];
	else {
		if($from=='module') $uin_cache[$from][$id]=getrowval("SELECT module_id, module_sname FROM main_module WHERE module_id=$id","module_sname");
		if($from=='table') $uin_cache[$from][$id]=getrowval("SELECT table_id, table_sname FROM main_table WHERE table_id=$id","table_sname");
		if($from=='part')  $uin_cache[$from][$id]=getrowval("SELECT part_id, part_sname FROM main_part WHERE part_id=$id","part_sname");
		if($from=='ex')  $uin_cache[$from][$id]=getrowval("SELECT ex_id, ex_sname FROM ex_module WHERE ex_id=$id","ex_sname");
		if(!isset($uin_cache[$from][$id])) return '';
		else {
			$uin_cache[$from][$id]='!'.$uin_cache[$from][$id];
			return $uin_cache[$from][$id];
		}
	}
}

function get_uin3($from,$uin,$where='',$what=''){
	global $uin_cache2;	
	//if(!empty($uin_cache2[$from][$uin])) return $uin_cache2[$from][$uin]; //здесь специально empty, т.к. в процессе добавления они могут замещаться
	// закомментировано т.к. если добавляется два раза один и тот же экземпляр, то объекты липнут по ссылкам к первому, а не к последнему
	if(!empty($uin) && $uin[0]=='!'){
		$uin2=substr($uin,1);
		$add='';
		if(!empty($where)) $add=' AND '.$where.'='.$what;
		if($from=='module') $uin_cache2[$from][$uin]=getrowval("SELECT module_id, module_sname FROM main_module WHERE module_sname='$uin2'".$add,"module_id");
		if($from=='table') $uin_cache2[$from][$uin]=getrowval("SELECT table_id, table_sname FROM main_table WHERE table_sname='$uin2'".$add,"table_id");
		if($from=='part')  $uin_cache2[$from][$uin]=getrowval("SELECT part_id, part_sname FROM main_part WHERE part_sname='$uin2'".$add,"part_id");
		if($from=='col')  $uin_cache2[$from][$uin]=getrowval("SELECT col_id, col_sname FROM main_col WHERE col_sname='$uin2'".$add,"col_id");
		if($from=='ex')  $uin_cache2[$from][$uin]=getrowval("SELECT ex_id, ex_sname FROM ex_module WHERE ex_sname='$uin2'".$add,"ex_id");
	} else {
		if($from=='module') $uin_cache2[$from][$uin]=getrowval("SELECT module_id, module_uin FROM main_module WHERE module_uin='$uin'","module_id");
		if($from=='table') $uin_cache2[$from][$uin]=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_uin='$uin'","table_id");
		if($from=='part')  $uin_cache2[$from][$uin]=getrowval("SELECT part_id, part_uin FROM main_part WHERE part_uin='$uin'","part_id");
		if($from=='col')  $uin_cache2[$from][$uin]=getrowval("SELECT col_id, col_uin FROM main_col WHERE col_uin='$uin'","col_id");
		if($from=='group')  $uin_cache2[$from][$uin]=getrowval("SELECT group_uin,auth_id FROM main_auth WHERE group_uin='$uin'","auth_id");
		if($from=='ex')  $uin_cache2[$from][$uin]=getrowval("SELECT ex_id, ex_uin FROM ex_module WHERE ex_uin='$uin' ORDER BY ex_id DESC","ex_id");
		if($from=='row') $uin_cache2[$from][$uin]=getrowval("SELECT row_id, row_uin FROM main_row WHERE row_uin='$uin' ORDER BY row_id DESC","row_id");
	}
	if(isset($uin_cache2[$from][$uin])) return $uin_cache2[$from][$uin];
	else return false;
}

function pack_user($id){
	global $user,$db;
	$s='|?|';
	if(check_user(-$id,'edit')){
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id",1,"main_auth");
		foreach($db->Record AS $var=>$value) $$var=$value;
		$groups=getall3($db,"SELECT * FROM auth_link WHERE link_user=$id","link_group");
		if(!empty($groups)) $groups=implode('|',getall3($db,"SELECT * FROM main_auth WHERE auth_id IN (".implode(',',$groups).") AND auth_type=1 AND group_uin!=''","group_uin"));
		return $auth_date.$s.$user_login.$s.$user_pwl.$s.$user_fixedip.$s.$user_email.$s.$user_pwlcode.$s.$user_name.$s.$user_lastlogin.$s.$groups;
	} else return '0';
}

function unpack_user($text){
	global $user,$db,$su_login;
	$s='|?|';
	if(!check_user(0,'reg')) return 0;
	$text=explode($s,$text);
	$auth_date=$text[0];
	$user_login=$text[1];
	$user_pwl=$text[2];
	$user_fixedip=$text[3];
	$user_email=$text[4];
	$user_pwlcode=$text[5];
	$user_name=$text[6];
	$user_lastlogin=$text[7];
	$groups=explode('|',$text[8]);
	$u=getrowval("SELECT * FROM main_auth WHERE user_login='$user_login'",'auth_id');
	if(!empty($u)){
		//тут можно сделать добавление групп, в которых данный пользователь не числется
		//а также делать сверку по емайлу или по паролю (пока это не стал делать, т.к. чисто теоретически юзер может успеть поменять пароль или мыло)
		return $u;
	}
	if(empty($su_login)) $su_login=cfg_extract('su_login');
	if($su_login==$user_login) return false;
	unset($su_login);
	$db->query("INSERT INTO main_auth (auth_type, auth_owner, auth_date, user_login, user_pwl, user_fixedip, user_email, user_pwlcode, user_name, user_lastlogin)
			VALUES (0, '".$user->id."', '$auth_date', '$user_login', '$user_pwl', '$user_fixedip', '$user_email', $user_pwlcode, '$user_name', '$user_lastlogin')",3,'main_auth');
	$sid=$db->Record["sid"];
	$GLOBALS["lu"]=$sid;
	if(!empty($groups)) foreach($groups AS $g){
		$g=get_uin3('group',$g);
		if(!empty($g) && check_group($g,'invite')){
			$db->query("INSERT INTO auth_link (link_user, link_group, link_expire, link_date, link_invite)
			VALUES ($sid, $g, '0000-00-00', '".date('Y-m-d')."', ".$user->id.")",3,"auth_link");
		}
	}
	return $sid;
}

function params_to_text($row,$type='row',$row_table=0,$row_module=0){
	global $db,$rlink,$exp_rows,$exp_rows2,$cols_cache,$rstart,$rend,$exp_folders;
	$sep2='|^|';
	if(!isset($cols_cache[$row_table][$row_module])){
		if(empty($row_table)) $cols_cache[$row_table][$row_module]=getall4($db,"SELECT * FROM main_col WHERE col_module=$row_module",'col_id');
		else $cols_cache[$row_table][$row_module]=getall4($db,"SELECT * FROM main_col WHERE col_table=$row_table",'col_id');
	}
	$cols=$cols_cache[$row_table][$row_module];
	$col_ids=Array();
	foreach($cols AS $col) $col_ids[$col["col_id"]]=$col["col_id"];
	//$vals=getall8($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col IN (".implode(',',$col_ids).")","value_col","value_value");
	
	if(empty($row_table)) $vals=getall8($db,"SELECT * FROM row_value WHERE value_row=$row AND value_table=0 AND value_col!=0","value_col","value_value");
	else  $vals=getall8($db,"SELECT * FROM row_value WHERE value_row=$row AND value_table>0 AND value_col!=0","value_col","value_value");
	
	//$vals=getall8($db,"SELECT * FROM row_value WHERE value_row=$row AND value_table=$row_table AND value_col!=0 AND value_module=$row_module","value_col","value_value");
	$rval='';
	$have_tpl=false;
	//foreach($cols AS $col){
	foreach($vals AS $col_id=>$val){
		//if(isset($vals[$col["col_id"]])) $val=$vals[$col["col_id"]]; else $val=Array('');
		//if(empty($val)) $val=Array('');
		if(empty($cols[$col_id])) $col=getrow($db,"SELECT * FROM main_col WHERE col_id=".$col_id);
		else $col=$cols[$col_id];
		$value=$val[0];
		//if($col["col_type"]==0 || $col["col_type"]==2) $rval[$col["col_id"]]=$value;
		if(!empty($col["col_tpl"])) $have_tpl=true;
		if($col["col_type"]==1 && ($col["col_link2"]==0 || count($val)==1)){
			if(!empty($value)){
				$value=$rstart.row_to_text($value,/*$step+*/1,1).$rend;
			}
			else if($col["col_link2"]==0) $value='0';
			else $value='';
		}
		if($col["col_type"]==1 && $col["col_link2"]==1 && count($val)>1){
 			if(!empty($val)){
 				$value=$rstart.rows_to_text($val,/*$step+*/1,1).$rend;
 			}
 			else $value='';
		}
		if($col["col_type"]==3 && !empty($value)) $value=file_to_data($value);
		if($col["col_type"]==4 && !empty($value)){
			$value=explode(':',$value);
			$v_module=get_uin2('module',$value[0]);
			$v_ex=get_uin2('ex',$value[1]);
			if(!empty($value[2])) $v_part=get_uin2('part',$value[2]); else $v_part=0;
			$value=$v_module.'|?|'.$v_ex;
			if(!empty($v_part)) $value.='|?|'.$v_part;
		}
		if($col["col_type"]==5 && !empty($value)) $value=pack_user($value);
		if($col["col_type"]==6 && !empty($value)){
			if(empty($exp_folders[$value])) $value=folder_to_data($value);			
			$exp_folders[$value]=1;
		}
		if(!empty($rval)) $rval.=$sep2;
		if(!empty($col["col_uin"])) $rval.=$col["col_uin"].'='.$value;
		else $rval.='!'.$col["col_sname"].'='.$value;
	}
	//foreach($vals AS $col_id=>$val_array){
	//	if(!isset($cols[$col_id]) && $col_id!=0){
	//		
	//	}
	//}
	if($have_tpl && $GLOBALS['collect_parts']){
		if($type!='row') $tmp=get_row_parts_links($row,Array(),1,true);
		else $tmp=get_row_parts_links($row);
		if(!empty($tmp)){
			if($type!='row') $GLOBALS['collected_parts'][-$row]=unserialize($tmp);
			else $GLOBALS['collected_parts'][$row]=unserialize($tmp);
		}
	}
	return $rval;
}

function row_to_text($row,$step=0,$from_link=false){
	global $db,$exp_rows,$exp_rows2,$rstart,$rend,$exp_folders,$is_export_started;
	
	$sep='|~|';
	if(is_object($row) && isset($row->id)) $row=$row->id;
	$row_pos=getrowval("SELECT ro_pos FROM row_owner WHERE row_id=".$row,"ro_pos");
	getrow($db,"SELECT * FROM main_row WHERE row_id=".$row,1,"main_row");
	foreach($db->Record AS $var=>$value) $$var=$value;
	
	//uin check&get
	if(!empty($row_uin) && isset($exp_rows[$row_uin]) || ($from_link && $is_export_started)) return '!'.$row_uin;
	if(empty($row_uin)){
		$row_uin=uuid();
		$db->query("UPDATE main_row SET row_uin='$row_uin' WHERE row_id=$row",3,"main_row");
	}
	$exp_rows[$row_uin]=$row;
	$exp_rows2[$row]=$row_uin;
	
	//рутина
	$res[]=$row_uin;
	$res[]=get_uin2('module',$row_module);
	$res[]=get_uin2('table',$row_table);
	$res[]=$row_enable;
	$row_sub_x='0';
	if($row_sub!=0){
		if(!empty($exp_rows2[$row_sub])) $row_sub_x=$exp_rows2[$row_sub];
		//if(empty($row_sub_x)) echo '! '.$row.' - '.$row_sub.'<br>';
	}
	$res[]=$row_sub_x;
	
	//подготовка столбцов
	$res[]=params_to_text($row,'row',$row_table,$row_module);
	
	//SubRows
	$srows=getall7($db,"SELECT * FROM row_owner WHERE owner_id=$row","row_id");
	$srs='';
	if(!empty($srows)){
		$srs=rows_to_text($srows,$step+1);
		if(!empty($srs)) $srs=$rstart.$srs.$rend;		
	}
	$res[]=$srs;
	
	//date
	$res[]=$modified_date;
	
	//linked rows (from uin:uin_string)
	$links_tmp=get_row_uin_links($row,false);
	$mod_links=Array();
	if(!empty($links_tmp)){
		$links=Array();
		foreach($links_tmp AS $row) if(empty($exp_rows2[$row])){
			if(strstr($row,':')){
				$tmp=explode(':',$row);
				$mod_links[]=Array('module'=>$tmp[0],'ex'=>$tmp[1]);
			} else $links[$row]=$row;
		}
		if(!empty($links)) $res[]=$rstart.rows_to_text($links).$rend;
	} else $res[]='';
	
	$res[]=serialize($mod_links);
	
	$res[]=$row_pos;
	
	return implode($sep,$res);
}

function rows_to_text($rows,$step=0,$from_link=false){
	$data='';
	foreach($rows AS $row)if(!empty($row)){
		if(!empty($data)) $data.='|*|';
		$x=row_to_text($row,$step,$from_link);
		if($x) $data.=$x;
	}
	return $data;
}

function text_to_rows($text,$tex=0,$owner=0,$own_tex=0,$id7=0,$just_link=false,$from_linked=0){
	global $rstart,$rend;
	$text=explodeA('|*|',$text,$rstart,$rend);
	$rows=Array();
	//echo 'tex: '.$tex.'; owner: '.$owner.', own_tex: '.$own_tex.', id7='.$id7.', just_link='.$just_link.', from_linked='.$from_linked.'<br>';
	if(!empty($text)) foreach($text AS $t){
		$rows[]=text_to_row($t,$tex,$owner,$own_tex,$id7,$just_link,$from_linked);
	}
	return $rows;
}

function text_to_params($data,$row_id,$row_table,$row_module,$ex=0,$tex=0){
	global $db,$cls_cache,$rstart,$rend,$rlink,$user,$mod_import_started,$todo_row_link,$todo_rows_link;
	
	$sep2='|^|';
	$sid=$row_id;

	$cols=explodeA($sep2,$data,$rstart,$rend);
	$cls=Array();
	$cls_i=Array();
	if(!empty($cols)) foreach($cols AS $col){
		$col=explodeA('=',$col,'','',1);
		$i=get_uin3('col',$col[0]/*,'col_table',$row_table*/);
		if(!empty($i)){
			$cls_i[$i]=$i;
			$cls[$i]=$col[1];
		}
	}	
	
	$cls_imp=implode(',',$cls_i);
	if(empty($cls_imp)) return false;
	if(!isset($cls_cache[$cls_imp])) $cls_cache[$cls_imp]=getall4($db,"SELECT * FROM main_col WHERE col_id IN (".$cls_imp.")","col_id");
	$cls_i=$cls_cache[$cls_imp];
	foreach($cls AS $col=>$value){
		$add=Array($value);
		if(empty($cls_i[$col])) $col=getrow($db,"SELECT * FROM main_col WHERE col_id=".$col);
		else $col=$cls_i[$col];
		//возможно get_tag-и нужно будет заменить на get_tagA
		$lnk=$row_module.'.'.$row_table.'.'.$sid.'.'.$col["col_id"];
		$c=$col["col_id"];
		if($col["col_type"]==1){
			if($mod_import_started){
				if($col["col_type"]==1 && $col["col_link2"]==0){
					//$xt=text_to_row(get_tag($value,$rstart,$rend),0,0,$tex,/*??*/0/*??*/,true,$lnk);
					$todo_row_link[]=Array('module'=>$row_module, 'table'=>$row_table, 'col'=>$c, 'row'=>$row_id, 'data'=>get_tag($value,$rstart,$rend), 'lnk'=>$lnk, 'own_tex'=>$tex);
					//$add[0]=$xt;
				}
				if($col["col_type"]==1 && $col["col_link2"]==1){
					$todo_rows_link[]=Array('module'=>$row_module, 'table'=>$row_table, 'col'=>$c, 'row'=>$row_id, 'data'=>get_tag($value,$rstart,$rend), 'lnk'=>$lnk, 'own_tex'=>$tex);
					//$add=text_to_rows(get_tag($value,$rstart,$rend),0,0,$tex,/*??*/0/*??*/,true,$lnk);
				}
				continue;
			} else {
				if($col["col_type"]==1 && empty($tex) && !empty($ex)){
					$tex=getrowval("SELECT * FROM ex_group WHERE ex_table=".$col['col_link']." AND ex_ex2=$ex","ex_ex1");
				}
				if($col["col_type"]==1 && $col["col_link2"]==0){
					$xt=text_to_row(get_tag($value,$rstart,$rend),0,0,$tex,0,true,$lnk);
					$add[0]=$xt;
				}
				if($col["col_type"]==1 && $col["col_link2"]==1){
					$add=text_to_rows(get_tag($value,$rstart,$rend),0,0,$tex,0,true,$lnk);
				}
			}
		}
		if($col["col_type"]==3) $add[0]=data_to_file($value);
		if($col["col_type"]==4 && !empty($value)){
			$value=explode('|?|',$value);
			$v_module=get_uin3('module',$value[0]);
			if(empty($v_module)){
				$v_module=complex_import_module($value[0]);
			}
			$v_ex=get_uin3('ex',$value[1],'ex_module',$v_module);
			//echo $value[1].'!'.$v_ex.'<br>';
			if(empty($v_ex) && !empty($v_module)){
				$v_ex=import_ex(0,$value[1],false,$v_module);
			}
			if(!empty($value[2])) $v_part=get_uin3('part',$value[2]); else $v_part=0;
			if(!empty($v_module) && !empty($v_ex)){
				$value=$v_module.':'.$v_ex;
				if(!empty($v_part)) $value.=':'.$v_part;
			} else $value='';
			$add[0]=$value;
		}
		if($col["col_type"]==5 && !empty($value)) $add[0]=unpack_user($value);		
		if($col["col_type"]==6 && !empty($value)){
			$add[0]=data_to_folder($value);
		}
		//echo count($add).'<br>';
		foreach($add AS $val){
			//это надо оптимизировать (сгруппировать)
			if($col["col_type"]==0) $val=prepend_value($val);
			$stop=false;
			if($col["col_type"]==1){
				if(empty($val)) $stop=true;
			}
			$sql="INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
							VALUES ($row_module, $row_table, $sid, $c, '$val')";
			if(!$stop) $db->query($sql,3,'row_value',__LINE__,__FILE__);
		}
	}
}

function text_to_row($text,$tex=0,$owner=0,$own_tex=0,$id7=0,$just_link=false,$from_linked=0){
	global $db,$rstart,$rend,$exp_rows,$rlink,$user,$import_links,$import_rows,$recheck_rows;
	$sep='|~|';
	$sep2='|^|';
	//echo '!'.$tex.'!<br>';
	if(empty($text)){
		return false;
	}
	if(!empty($from_linked)){
		$tmp=explode('.',$from_linked);
		$from_linked_id=$tmp[2];
	}
	if(!empty($text) && $text[0]=='!'){
		$sid=get_uin3('row',substr($text,1));//getrowval("SELECT * FROM main_row WHERE row_uin='".substr($text,1)."'","row_id");
		if(empty($sid) && !empty($from_linked)){
			$rcnt=count($recheck_rows);
			$recheck_rows[$rcnt]["link"]=$from_linked;
			$recheck_rows[$rcnt]["uin"]=substr($text,1);
			return 0;
		}
		if($tex==0 && $owner==0){
			//это по сути то же, что just link
			if($just_link) return $sid;
			if(!empty($from_linked) && !empty($sid)){
				seek_rlink($from_linked_id);
				seek_rlink($sid);
				$o_exs=get_ex_all($rlink[$from_linked_id]->tex);
				$n_exs=get_ex_all($rlink[$sid]->tex);
				if(!empty($o_exs)) foreach($o_exs AS $val) if(!empty($n_exs[$val])) return $sid;
				//если экземпляры не пересекаются, то
				//переходим к процедуре описанной ниже и добавляем её для нашего экземпляра
			} else return $sid;
		}
		//добавляем ссылку (а нужна ли она тут?) - нужна, т.к. если не нужна, то будет выход из функции, о котором написано выше
		if(empty($sid)) return 0;
		seek_rlink($sid);
		$table_owner=$rlink[$sid]->rsub;//это стоит занулить
		$tex=$rlink[$sid]->tex;
		$ex=get_ex2($tex);
		$row_table=$rlink[$sid]->table;
		$row_module=$rlink[$sid]->module;
		if(!check_operation('add',0,$table_owner,$ex,$row_table)) return false;
		$owner_table=$row_table;//0
		$owner_module=$row_module;//0
		if(empty($owner) && !empty($from_linked_id)){
			//if(!empty($from_linked_id)){
				seek_rlink($from_linked_id);
				$tex=seek_match_tex($rlink[$from_linked_id]->tex,$row_table);
			//}
			getrow($db,"SELECT * FROM row_owner WHERE ro_ex=$tex AND row_id=$sid AND owner_id=$owner AND owner_table=$owner_table AND owner_module=$owner_module AND row_table=$row_table AND row_module=$row_module AND ro_sub=$table_owner");
			if(!empty($db->Record)){
				//echo '1';
				return $sid; //если такой уже есть в этом экземпляре
			}
		}
		if(!empty($owner)){
			seek_rlink($owner);
			$owner_table=$rlink[$owner]->table;
			if(!empty($rlilnk[$owner]->module)) $owner_module=$rlilnk[$owner]->module;
			$tex=seek_match_tex($rlink[$owner]->tex,$row_table);
		}
		
		if(getrowval("SELECT ro_id FROM row_owner WHERE ro_ex=$tex AND row_id=$sid AND owner_id=$owner AND ro_sub=$table_owner","ro_id")==false){			
			getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$tex AND row_table=$row_table AND owner_id=$owner AND owner_table=$owner_table AND ro_sub=$table_owner AND owner_module=$row_module",1,'row_owner');
			if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;				
			$import_links++;		
			$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, ro_sub, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
				VALUES ($pos, $tex, $sid, $table_owner, $row_module, $row_table, $owner, $owner_table, $owner_module, ".$user->id.")",3,'row_owner');		
		}
		//echo '2';
		return $sid;
	}
	$text=explodeA($sep,$text,$rstart,$rend);
	$row_uin=$text[0];
	if(!empty($row_uin)){
		//getrow($db,"SELECT * FROM main_row WHERE row_uin='".$row_uin."'");
		$tuir=get_uin3('row',$row_uin);
		if(!empty($tuir) && (empty($tex) || $just_link) /*сделано специально, чтобы можно было постить из системы в системы*/){
			//echo '3';
			return $tuir;//было просто false
		}
		if(!empty($tuir) && !empty($tex)){
			//$row_uin=uuin(); //тогда не будет работать подтягивание связанных строк через uin:uin_string
		}
	}

	if($owner!=0){
		global $rlink;
		seek_rlink($owner);
		$row_table=get_uin3('table',$text[2]);
		if(empty($row_table) || empty($text[2])){
			if(!empty($id7)) $row_table=$id7;//опасно
			if(empty($row_table)) $row_table=getrowval("SELECT ex_id, ex_table FROM ex_table WHERE ex_id=$tex","ex_table");//опасно
			if(empty($row_table)) $row_table=$rlink[$owner]->table;//ещё опасней, т.к. если я втыкаю в подтаблицу, то там идёт не совпадения с id7
		}
		$row_module=$rlink[$owner]->module;
		$tex=$rlink[$owner]->tex;
		$owner_table=$rlink[$owner]->table;
		if($rlink[$owner]->table==$row_table) $rsub=$rlink[$owner]->rsub;
		else $rsub=$owner;
		$owner_module=$row_module;
		if(!empty($id7) && $row_table!=$id7){
			//echo '4';
			return false;
		}
		//выше сделал ^ //тут каким-то макаром нужно проверку, ведь не стоит забывать о том, что я могу запастить в sub с id6!=0, а в импорте таблица может не соответствовать текущей (id7)
	} else if($tex!=0){
		global $tex_cache;
		if(empty($tex_cache[$tex])){
			getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=$tex");
			$tex_cache[$tex]=new stdClass();
			$tex_cache[$tex]->table=$db->Record["ex_table"];
			$tex_cache[$tex]->module=$db->Record["ex_module"];
		}
		$row_table=$tex_cache[$tex]->table;
		$row_module=$tex_cache[$tex]->module;
		$owner_table=$row_table;
		$owner_module=$row_module;
		$real_table=get_uin3('table',$text[2]);
		$rsub=0;
		if(!empty($real_table) && $row_table!=$real_table){
			//echo '5';
			return false;
		}
	} else {
		$row_module=get_uin3('module',$text[1]);
		$row_table=get_uin3('table',$text[2]);
		if(empty($row_module)) return false;
		if(empty($row_table)) return false;	
		if(!empty($own_tex)){
			$exx=getrowval("SELECT * FROM ex_group WHERE ex_ex1=$own_tex","ex_ex2");
			if(!empty($exx)) $tex=getrowval("SELECT * FROM ex_group WHERE ex_ex2=$exx AND ex_table=$row_table","ex_ex1");
		}
		if(empty($tex)) $tex=getrowval("SELECT * FROM ex_group WHERE ex_module=$row_module AND ex_table=$row_table ORDER BY ex_id DESC","ex_ex1");
		$owner_table=$row_table;
		$owner_module=$row_module;
		$rsub=0;
	}
	if(empty($tex)){
		//echo '6';
		return false;
	}
	$ex=get_ex2($tex);
	$row_enable=$text[3];
	$table_owner=$text[4];
	if(!empty($table_owner)){
		if(isset($exp_rows[$table_owner])) $table_owner=$exp_rows[$table_owner];
		else $table_owner=get_uin3('row',$table_owner);//getrowval("SELECT * FROM main_row WHERE row_uin='$table_owner'","row_id");
	}
	
	if(!check_operation('add',0,$table_owner,$ex,$row_table)){
		//echo '7';
		return false;
	}
	
	$xn=get_uin3('row',$row_uin);//getrowval("SELECT * FROM main_row WHERE row_uin='$row_uin'","row_id");
	$found=false;
	if(empty($xn) || !$just_link){
		$import_rows++;
		$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user, row_uin)
				VALUES ($row_module, $row_table, $tex, $rsub, ".$user->id.", '".$row_uin."')",3,'main_row');
		getrow($db,'SELECT LAST_INSERT_ID() as sid');
		$sid=$db->Record['sid'];
		global $change_uin;
		if(!empty($xn)) $change_uin[$sid]=$row_uin;
		$GLOBALS['li']=$sid;
	} else {
		$found=true;
		$sid=$xn;
		return $sid;//попробуем пока просто возвращать строку, если она уже имелась, без загрузки values-ов
		//тут надо предусматреть такой вариант: предыдущая строка могла быть связана с этой, и тогда эта строка уже подгружена и на неё не надо делать ссылку, как бы предусмотреть такой вариант...
		//можно сделать что-то типа if($owner!=0 || $tex!=0) тогда фиксировать эту строку и в дальнейшем не добавлять... хотя в случае с предыдущим примером, как раз строка не зафиксируется, потому что там и tex и owner будут равны 0
	}
	
	if(empty($table_owner)) $table_owner='0';
	if(!empty($text[10])){
		$pos=$text[10];
	} else {
		getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$tex AND row_table=$row_table AND owner_id=$owner AND owner_table=$owner_table AND ro_sub=$table_owner AND owner_module=$row_module",1,'row_owner');
		if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
	}
	$import_links++;

	$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, ro_sub, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
			VALUES ($pos, $tex, $sid, $rsub, $row_module, $row_table, $owner, $owner_table, $owner_module, ".$user->id.")",3,'row_owner');
	
	text_to_params($text[5],$sid,$row_table,$row_module,0,$tex);	

	$subs=$text[6];
	if(!empty($subs)) text_to_rows(get_tag($subs,$rstart,$rend),$tex,$sid);
	
	update_row_state($sid,$text[7]);

	if(!empty($text[9])){
		$mds=unserialize($text[9]);
		if(!empty($mds)) foreach($mds AS $m){
			import_modex($m['module'],$m['ex']);
		}
	}
	
	if(!empty($text[8])){
		text_to_rows(get_tag($text[8],$rstart,$rend),/*$tex*/0,0,0,0,true);
	}
	
	reset_row_part_links($sid);

	$exp_rows[$row_uin]=$sid;
	return $sid;
}

?>