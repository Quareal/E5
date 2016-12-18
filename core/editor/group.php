<?php

define_lng('group');

global $user;
if(empty($fmod))	$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 ORDER BY group_name",1,'main_auth');
else				$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 AND group_module=$fmod ORDER BY group_name",1,'main_auth');
$madd='';$madd2='';$madd3='';$mod2='';
if(!empty($fmod)){
	$madd='&amp;fmod='.$fmod;
	$madd2='<input type="hidden" name="fmod" value="'.$fmod.'">';
	$madd3='?fmod='.$fmod;
	getrow($db,"SELECT * FROM main_module WHERE module_id=$fmod");
	$mod2=$db->Record["module_name"];
}
if(!empty($group)){
	$fmod=getrowval("SELECT * FROM main_auth WHERE auth_id=$group","group_module");
}
if(empty($fmod)) $fmod=0;
$tmm2=$m;

//=======================
//  Приглашение пользователя
//=======================
if(!empty($action) && $action=='invite' && isset($_GET["group"])){
	invite_user($_GET["group"],$auth_id,0,$fmod);
	$action='';
	$vars['title']=lng('User successfully invited');
	echo shell_tpl_admin('block/message_box', $vars);
}

//=======================
//  Добавление пользователя
//=======================
if(!empty($action) && $action=='add2'){
	$user_pwl=$_POST["user_pwl"];
	$user_login=$_POST["user_login"];
	
	$res=user_reg($user_login,$user_pwl,$user_name,$user_email,'','',0,0,$user_fixedip,$user_pwlcode,$fmod,Array(),false,$user_session_lifetime,$user_session_multy);
	if($res==2){
		$vars['title']=lng('User with this login is already registered in the system');
		$vars['msg']=lng('Please go back and enter the login other than').' '.$user_login;
		echo shell_tpl_admin('block/message_box', $vars);
	}
	if($GLOBALS["lu"]!=0 && $res==1){
		$sid=$GLOBALS["lu"];
		if(!empty($gbu)) foreach($gbu AS $var=>$value)if(!empty($value) && $var!=-100 && check_group($var,'invite',0,$fmod) && $var!=0){
			$db->query("INSERT INTO auth_link (link_user, link_group, link_expire, link_date, link_invite)
						VALUES ($sid, $var, '0000-00-00', '".date('Y-m-d')."', ".$user->id.")",3,"auth_link");
		}
		$vars['title']=lng('User successfully added');
		echo shell_tpl_admin('block/message_box', $vars);
	}
	unset($su_login);
	$action='';
}

//=======================
//  Редактирование пользователя
//=======================
if(!isset($u_owner)) $u_owner=0;
if(!empty($action) && $action=='edit2') if(is_array($u_owner)) $u_owner=implode(',',array_keys($u_owner));
if(!empty($action) && $action=='edit2' && !empty($id) && isset($group) && (check_user(-$id,'edit',$u_owner,0,0,$fmod) || $id==$user->id)){
	$user_pwl=$_POST["user_pwl"];
	$user_login=$_POST["user_login"];
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$user_login' AND auth_id!=$id",1,"main_auth");
	if(empty($su_login)) $su_login=cfg_extract('su_login');
	if(!empty($db->Record) || (!empty($su_login) && $su_login==$user_login)){
		$vars['title']=lng('User with this login is already registered in the system');
		$vars['msg']=lng('Please go back and enter the login other than').' '.$user_login;
		echo shell_tpl_admin('block/message_box', $vars);
	} else {
		if(!empty($user_pwl)){
			if($user_pwlcode) $user_pwl=md5(md5($user_pwl).get_protection_code());
			$upd=' user_pwl=\''.$user_pwl.'\', user_pwlcode='.$user_pwlcode.', ';
		} else $upd='';
		$db->query("UPDATE main_auth SET
				".$upd."
				user_login='$user_login',
				user_name='$user_name',
				user_email='$user_email',
				user_fixedip='$user_fixedip',
				auth_owner='$u_owner',
				session_lifetime=$user_session_lifetime,
				session_multy=$user_session_multy
				WHERE auth_id=$id");
		$links2=getall($db,"SELECT * FROM auth_link WHERE link_user=$id");
		$lnk=Array();
		if(!empty($links2)) foreach($links2 AS $link2) if(!empty($link2["auth_id"])) $lnk[$link2["auth_id"]]=$link2;
		$db->query("DELETE FROM auth_link WHERE link_user=$id");
		$sid=$id;
		if(!empty($gbu)) foreach($gbu AS $var=>$value)if(!empty($value) && $var!=-100){
			$d=date('Y-m-d');
			$d2='0000-00-00';
			if(!empty($lnk[$var])) $d=$lnk[$var]["link_date"];
			if(!empty($lnk[$var])) $d2=$lnk[$var]["link_expire"];
			$db->query("INSERT INTO auth_link (link_user, link_group, link_expire, link_date, link_invite)
						VALUES ($sid, $var, '$d2', '$d', ".$user->id.")",3,"auth_link");
		}
		$vars['title']=lng('User updated successfully');
		echo shell_tpl_admin('block/message_box', $vars);
	}
	unset($su_login);
	$action='';
}

//=======================
//  Добавление группы
//=======================
if(!empty($action) && $action=='add' &&  check_group(0,'add',0,$fmod)){
	if(!empty($fmod)) update_module_state($fmod);
	getrow($db,"SELECT * FROM main_auth WHERE group_name='$group_name' OR group_sname='$group_sname'",1,"main_auth");
	if(!empty($db->Record)){
		$vars['title']=lng('Unable to add a group');
		$vars['msg']=lng('The group with the same name already exists');
		echo shell_tpl_admin('block/message_box', $vars);
	} else {
		$group_uin=uuin();
		$db->query("INSERT INTO main_auth (auth_type, auth_owner, auth_date, group_name, group_module, group_uin, group_sname, session_lifetime, session_multy)
				VALUES (1, ".$user->id.", '".date('Y-m-d')."','$group_name', $group_module, '$group_uin', '$group_sname', $user_session_lifetime, $user_session_multy)",3,'main_auth');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
		if(empty($fmod))	$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 ORDER BY group_name",1,'main_auth');
		else				$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 AND group_module=$fmod ORDER BY group_name",1,'main_auth');
		$action='';
		global $user;
		if(!$user->super){
			$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_rules,perm_edit,perm_del)
							VALUES	(1,6,$user->id,$sid,2,2,2,2)",3,"auth_perm");
			$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_rules,perm_invite,perm_leave)
							VALUES	(1,8,$user->id,$sid,2,2,2,2)",3,"auth_perm");
		}
		$vars['title']=lng('Group successfully added');
		echo shell_tpl_admin('block/message_box', $vars);
	}
}

//=======================
//  Удаление группы подтверждение
//=======================
if(!empty($action) && $action=='del_g' && !empty($id) && empty($group) && check_group($id,'del',0,$fmod)){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id AND auth_type=1",1,"main_auth");
	$name=' "'.$db->Record["group_name"].'"';
	$vars['type']='Attension';
	$vars['action']='group?id='.$id.'&action=del_g_confirm';
	$vars['btns']['smb1']=Array(lng('Yes'),lng('No'));
	$vars['form_body']=$madd2.get_form_protection_key('group',1,1);
	$vars['title']=lng('Do you really want to delete group').' '.$name.'?';
	$vars['msg']=lng('After this operation, users will go into the "Not in a group", if you want to delete all users - first do so via the menu editing users, and then delete the group. Operation is not reversible.');
	echo shell_tpl_admin('block/confirm_box', $vars);
	unset($action);
}

//=======================
//  Удаление группы
//=======================
if(!empty($action) && $action=='del_g_confirm' && (!empty($id) && !empty($smb1) && $smb1==lng('No'))){ $action='';}
if(!empty($action) && $action=='del_g_confirm' && !empty($id) && empty($group) && check_group($id,'del',0,$fmod) && $smb1==lng('Yes') && check_form_protection_key($_POST['key'],'group',1)){
	del_group($id);
	if(!empty($fmod)) update_module_state($fmod);
	if(empty($fmod))	$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 ORDER BY group_name",1,'main_auth');
	else				$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 AND group_module=$fmod ORDER BY group_name",1,'main_auth');
	$action='';
	$vars['title']=lng('Group successfully removed');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Удаление пользователя
//=======================
if(!empty($_POST["group"])) $_GET["group"]=$_POST["group"];
if(!empty($action) && $action=='del' && (!empty($id) && !empty($smb1) && $smb1==lng('No'))){ $action='';}
if(!empty($action) && $action=='del' && ((!empty($id) && !empty($smb1) && $smb1==lng('Yes')) || isset($chk)) && isset($group) && check_form_protection_key($_POST['key'],'group',1)){
	if(isset($chk)){
		foreach($chk AS $vid=>$checked) if($checked && check_user(-$vid,'del',0,0,0,$fmod)) del_group($vid);
	} else {
		if(check_user(-$id,'del',0,0,0,$fmod)) del_group($id);
		if(!empty($r) && is_array($r)) foreach($r AS $row_id=>$enable) if($enable){
			global $rlink;
			seek_rlink($row_id);
			if(isset($rlink[$row_id])){
				$r=$rlink[$row_id];
				if(check_row($r->id,$r->table,get_ex2($r->tex),'del',$r->user,$r->users)) del_row($r->id);
			}
		}
	}
	if(empty($fmod))	$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 ORDER BY group_name",1,'main_auth');
	else				$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 AND group_module=$fmod ORDER BY group_name",1,'main_auth');
	$action='';
	$vars['title']=lng('User successfully droped');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Подготовка к удалению пользователя
//=======================
if(!empty($action) && $action=='del_confirm' && !empty($id) && check_user(-$id,'del',0,0,0,$fmod)){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id",1,"main_auth");
	$name=$db->Record["user_name"].' ('.$db->Record["user_login"].')';
		
	//следующие запросы могут быть очень объёмными
	$cols=getall($db,"SELECT col_type,col_id FROM main_col WHERE col_type=5",1,"main_col");
	$cols2=Array();
	if(!empty($cols)) foreach($cols AS $col) $cols2[$col["col_id"]]=$col["col_id"];
	$cols=implode(',',$cols2);
	$addon='';
	if(!empty($cols)){
		$vals=getall($db,"SELECT value_col,value_value,value_row FROM row_value WHERE value_col IN ($cols) AND value_value='$id'",1,"value_value");
		$rows=Array();$rows2=Array();$rtables=Array();
		if(!empty($vals)) foreach($vals AS $val)if(empty($rows[$val["value_row"]])){
			global $rlink;
			$rows[$val["value_row"]]=1;
			seek_rlink($val["value_row"]);
			if(isset($rlink[$val["value_row"]])){
				$r=$rlink[$val["value_row"]];
				if(check_row($r->id,$r->table,get_ex2($r->tex),'del',$r->user,$r->users)){
					$rows2[$r->table][$r->id]=get_basename($r->id);
				}
			}
		}
		if(!empty($rows2)){
			global $tables;
			prep_tables();
			$addon.='<br>'.lng('Were found objects associated with that user, that you can remove').':';
			$addon.='<div class="sub_box">';
			$first=true;
			foreach($rows2 AS $table=>$rows){
				if(!$first) $addon.='<br>';
				$first=false;
				$addon.=lng('Table').' "<b>'.$tables[$table]->name.'</b>"';
				foreach($rows AS $row=>$name){
					$addon.='<br><input type="checkbox" class="checkbox" name="r['.$row.']"> '.$name;
				}
			}
			$addon.='</div><br>';
		}
	}
	
	$vars['type']='Attension';
	$vars['action']='group';
	$vars['hidden']['id']=$id;
	$vars['hidden']['group']=$group;
	$vars['hidden']['action']='del';
	$vars['btns']['smb1']=Array(lng('Yes'),lng('No'));
	$vars['form_body']=$madd2.get_form_protection_key('group',1,1).$addon;
	$addon='';
	$vars['title']=lng('Do you really want to drop user').' '.$name.'?';
	echo shell_tpl_admin('block/confirm_box', $vars);
	
	$action='';
}

//=======================
//  Удаление из группы
//=======================
if(!empty($action) && $action=='delgroup' && (!empty($id) || isset($chk)) && !empty($group) && check_group($group,"leave",0,$fmod)){
	if(isset($chk)){
		foreach($chk AS $vid=>$checked) if($checked){
				$db->query("DELETE FROM auth_link WHERE link_group=$group AND link_user=$vid",3,"auth_link");
		}
	} else {
		$db->query("DELETE FROM auth_link WHERE link_group=$group AND link_user=$id",3,"auth_link");
	}
	$action='';
	$vars['title']=lng('The user has been successfully excluded from the group');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Деактивация
//=======================
if(!empty($action) && $action=='deactive' && !empty($id) && isset($group) && check_user($id,"edit",0,0,0,$fmod)){
	$db->query("UPDATE main_auth SET auth_enable=0 WHERE auth_id=$id",3,"main_auth");
	$action='';
	$vars['title']=lng('User deactivated');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Активация
//=======================
if(!empty($action) && $action=='active' && !empty($id) && isset($group) && check_user($id,"edit",0,0,0,$fmod)){
	$db->query("UPDATE main_auth SET auth_enable=1 WHERE auth_id=$id",3,"main_auth");
	$action='';
	$vars['title']=lng('User activated');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Редактирование группы
//=======================
if(!empty($action) && $action=='edit' && isset($id) && !isset($group) &&  check_group($id,'edit',0,$fmod)){
	//тут нужна проверка на группы с такими же именами
	$db->query("UPDATE main_auth SET
		group_name='$group_name',
		group_sname='$group_sname',
		group_module=$group_module,
		session_lifetime=$user_session_lifetime,
		session_multy=$user_session_multy
	WHERE auth_id=$id",3,'main_auth');
	$action='';
	if(empty($fmod))	$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 ORDER BY group_name",1,'main_auth');
	else				$m=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0 AND group_module=$fmod ORDER BY group_name",1,'main_auth');
	$vars['title']=lng('Group modified');
	echo shell_tpl_admin('block/confirm_box', $vars);
}

//=======================
//  Вывод таблицы пользователей в группе
//=======================

if(isset($group)){
	$vars=Array('url'=>'group'.$madd3);
	echo shell_tpl_admin('block/go_back_box',$vars);
	if(check_group($group,'rules',0,$fmod)){
		$vars=Array();
		$vars['url']='perm?id='.$group.$madd;
		$vars['go_back_icon']='edit';
		if($group!=0) $vars['go_back_text']=lng('permission').' '.lng('groups');
		else $vars['go_back_text']=lng('permission').' '.lng('default');
		echo shell_tpl_admin('block/go_back_box',$vars);
	}
	
	sdefine('td_check',0);
	sdefine('td_login',1);
	sdefine('td_name',2);
	sdefine('td_email',3);
	sdefine('td_ip',4);
	sdefine('td_permission',5);
	sdefine('td_actions',6);
	$vars['rows']=Array();
	$vars['th']=Array('<input type="checkbox" class="checkbox1b" OnClick="chkall(this);show(\'cpanel\');">',lng('Login'),lng('Name'),lng('Email'),lng('Last IP'),lng('Permission'),lng('Actions'));	
	$vars['pre_html']='<br><form action="group" id="form" method="post"><input type="hidden" id="action" name="action">'.$madd2.get_form_protection_key('group',1,1);
	if(!empty($group)) $vars['pre_html'].='<input type="hidden" name="group" value="'.$group.'">';
	$after='';
	
	if($group!=0) $tus=getall($db,"SELECT * FROM auth_link WHERE link_group=$group",1,"auth_link");
	else $tus=getall($db,"SELECT * FROM auth_link",1,"auth_link");
	$tmp=Array();$tmp2='';
	if(!empty($tus)) foreach($tus AS $tu)if(empty($tmp[$tu["link_user"]])){
		$tmp[$tu["link_user"]]=1;
		if(!empty($tmp2)) $tmp2.=',';
		$tmp2.=$tu["link_user"];
	}
	if($group==0 && empty($tmp2)) $tmp2='-1';
	if(!empty($tmp2)){
		if($group!=0) $us=getall($db,"SELECT * FROM main_auth WHERE auth_type=0 AND auth_id IN ($tmp2)",1,"main_auth");
		else $us=getall($db,"SELECT * FROM main_auth WHERE auth_type=0 AND !(auth_id IN ($tmp2))",1,"main_auth");
	}
	if(!empty($us))if(check_user($group,'view',0,0,0,$fmod) || $group==0){
		$tus=Array();
		$uids=Array();
		$visits=Array();
		foreach($us AS $u)if((check_user($group,'view',$u["auth_owner"],0,0,$fmod) || $group==0) && (check_user(-$u["auth_id"],"view",$u["auth_owner"],0,0,$fmod) || ($group==0 && check_user(-$u["auth_id"],"view",$u["auth_owner"],1,0,$fmod)))  ){
			$tus[]=$u;
			$uids[$u['auth_id']]=$u['auth_id'];
		}
		if(!empty($uids)){
			$visits=getall6($db,"SELECT session_last, session_auth FROM auth_session WHERE session_auth IN (".implode(',',$uids).") AND session_active=1 ORDER BY session_last",'session_auth','session_last');
		}
		//foreach($us AS $u)if((check_user($group,'view',$u["auth_owner"],0,0,$fmod) || $group==0) && (check_user(-$u["auth_id"],"view",$u["auth_owner"],0,0,$fmod) || ($group==0 && check_user(-$u["auth_id"],"view",$u["auth_owner"],1,0,$fmod)))  ){
		if(!empty($tus)) foreach($tus AS $u){
			$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];	
			$r['id']='r'.$u["auth_id"];
			if((check_user($group,'rules',0,0,0,$fmod) || $group==0) && check_user(-$u["auth_id"],"edit",$u["auth_owner"],0,0,$fmod)) $rc[td_check]='<input type="checkbox" class="checkbox1" id="'.$u["auth_id"].'" name="chk['.$u["auth_id"].']" OnClick="show(\'cpanel\');selr(\''.$u["auth_id"].'\');">';
			else $rc[td_check]='';
			$rc[td_login]=si('user',5,0,lng('User')).$u["user_login"];
			if(!empty($visits[$u['auth_id']])){
				$u["user_lastlogin"]=get_min($visits[$u['auth_id']]);
			}
			if($u["user_lastlogin"]>$GLOBALS["cur_min"]-5) $rc[td_login].=' <span class="user_online">('.lng('online').')</span>'; else {
				$rc[td_login].=' <span class="user_offline">('.lng('was').' ';
				$lim=$GLOBALS["cur_min"]-$u["user_lastlogin"];
				if($lim<60) $rc[td_login].=$lim.lng('min');
				else if($lim<24*60) $rc[td_login].=floor($lim/60).lng('h');
				else $rc[td_login].=floor($lim/(60*24)).lng('d');
				$rc[td_login].=' '.lng('ago').')</span>';
			}
			$rc[td_name]=$u['user_name'];
			$rc[td_email]=$u['user_email'];
			$rc[td_ip]=$u['user_ip'];
			if((check_user($group,'rules',0,0,0,$fmod) || $group==0) && check_user(-$u["auth_id"],"edit",$u["auth_owner"],0,0,$fmod)) $rc[td_permission]='<a href="perm?id='.$u["auth_id"].$madd.'">'.lng('Editing').'</a>';
			else $rc[td_permission]='';
			$rc[td_actions]='';
			if($u["auth_enable"]) $rc[td_actions].=se('mail','mail?to='.$u["auth_id"].$madd);
			if((check_user($group,'edit',0,0,0,$fmod) || $group==0) && check_user(-$u["auth_id"],"edit",$u["auth_owner"],0,0,$fmod)){
				if($u["auth_enable"]) $rc[td_actions].=se('deactivate','group?group='.$group.'&amp;id='.$u["auth_id"].'&amp;action=deactive'.$madd,'',' onclick="return(confirm(\''.lng('Are you sure?').'\'))"');
				if(!$u["auth_enable"]) $rc[td_actions].=se('activate','group?group='.$group.'&amp;id='.$u["auth_id"].'&amp;action=active'.$madd,'','"  onclick="return(confirm(\''.lng('Are you sure?').'\'))"');
			}
			if($group!=0 && check_group($group,'leave',0,$fmod)) $rc[td_actions].=se('out','group?group='.$group.'&amp;id='.$u["auth_id"].'&amp;action=delgroup'.$madd,'','"  onclick="return(confirm(\''.lng('Are you sure?').'\'))"');
			if((check_user($group,'edit',0,0,0,$fmod) || $group==0) && check_user(-$u["auth_id"],"edit",$u["auth_owner"],0,0,$fmod)){		
				$rc[td_actions].=se('edit','group?group='.$group.'&amp;id='.$u["auth_id"].'&amp;action=edit'.$madd.'#edit');
			}
			if((check_user($group,'del',0,0,0,$fmod) || $group==0) && check_user(-$u["auth_id"],"del",$u["auth_owner"],0,0,$fmod)) $rc[td_actions].=se('del','group?group='.$group.'&amp;id='.$u["auth_id"].'&amp;action=del_confirm'.$madd,'','');
		}	
		
		$after.='<script>function chkall(chb){';
		$after.='var obj=document.getElementsByClassName("checkbox1");';
		$after.='if(obj.length==0) obj=document.getElementsByClassName("checkbox2"); if(obj.length==0) return "";';
		$after.='for (var key in obj) {
				 var val = obj[key];
				 val.checked=chb.checked;
				 selr2(val.id,chb.checked);
			 }';
		$after.='}</script>';
		$after.='<input type="submit" name="smb" style="width: 0px; border: 0px; padding: 0px; margin: 0px;" value=""></form>';
		$after.='<div id="cpanel" style="display: none; margin-top: 10px;">'.lng('With selected').': <span class="link" OnClick="document.getElementById(\'action\').value=\'delgroup\'; document.getElementById(\'form\').submit();">'.si('out').'</span> <span class="link" OnClick="document.getElementById(\'action\').value=\'del\'; document.getElementById(\'form\').submit();">'.si('del').'</span></div>';
	}
	$vars['post_html']=$after.show_se();
	echo shell_tpl_admin('block/table',$vars);
}

//=======================
//  Вывод таблицы групп
//=======================
if(!isset($group)){
	if(!empty($fmod)){
		$vars=Array();
		$vars['url']='mod_main?id='.$fmod;
		$vars['go_back_icon']='back';
		$vars['go_back_text']=lng('Back to module').' «'.lng('groups').'»';
		echo shell_tpl_admin('block/go_back_box',$vars);
	}
	if(empty($fmod) && $user->super){
		$vars=Array();
		$vars['url']='perm?id=0';
		$vars['go_back_icon']='edit';
		$vars['go_back_text']=lng('Default permissions');
		echo shell_tpl_admin('block/go_back_box',$vars);
	}
	if(!empty($fmod)){
		$vars=Array();
		$vars['url']='perm?id=0&fmod='.$fmod;
		$vars['go_back_icon']='edit';
		$vars['go_back_text']=lng('Default module permissions');
		echo shell_tpl_admin('block/go_back_box',$vars);
	}

	sdefine('td_name',0);
	sdefine('td_users',1);
	sdefine('td_module',2);
	sdefine('td_permission',3);
	sdefine('td_actions',4);
	$vars['rows']=Array();
	if(empty($fmod)) $vars['th']=Array(lng('Name'),lng('Users'),lng('Module'),lng('Permission'),lng('Actions'));
	else $vars['th']=Array(lng('Name'),lng('Users'),lng('Permission'),lng('Actions'));
	$vars['pre_html']='<br>';
	
	if(!empty($m)) foreach($m AS $tcm)if(check_group($tcm["auth_id"],'view',0,$fmod) && ($tcm["group_module"]==0 || !strstr(getrowval("SELECT module_sname FROM main_module WHERE module_id=".$tcm["group_module"],"module_sname"),'_for_uninstall'))){
		$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];	
		$rc[td_name]=si('group').$tcm["group_name"];
		if($tcm["group_module"]!=0){
			getrow($db,"SELECT * FROM main_module WHERE module_id=".$tcm["group_module"],1,"main_module");
			$rc[td_name].=' ('.$db->Record["module_name"].')';
		}
		getrow($db,"SELECT count(*) AS count FROM auth_link WHERE link_group=".$tcm["auth_id"],1,"auth_link");
		if(!empty($db->Record["count"])) $su=' ('.$db->Record["count"].')'; else $su='';
		if(!$user->super) $su='';
		if(check_user($tcm["auth_id"],'view',0,0,0,$fmod)) $rc[td_users]='<a href="group?group='.$tcm["auth_id"].$madd.'">'.lng('Editing').$su.'</a>';
		else $rc[td_users]='';
		if(empty($fmod)){
			if(!empty($tcm["group_module"])) $rc[td_module]='<a href="mod_main?id='.$tcm["group_module"].'">'.getrowval("SELECT module_name FROM main_module WHERE module_id=".$tcm["group_module"],'module_name').'</a>';
			else $rc[td_module]=lng('none');
		}
		if(check_group($tcm["auth_id"],'rules',0,$fmod)) $rc[td_permission]='<a href="perm?id='.$tcm["auth_id"].$madd.'">'.lng('Editing').'</a>';
		else $rc[td_permission]='';
		$rc[td_actions]='';
		$rc[td_actions].=se('mail','mail?to='.$tcm["auth_id"].$madd,lng('Write a letter to group'));
		if(check_group($tcm["auth_id"],'edit',0,$fmod)) $rc[td_actions].=se('edit','group?id='.$tcm["auth_id"].'&amp;action=edit_form'.$madd.'#edit_form');
		if(check_group($tcm["auth_id"],'del',0,$fmod)) $rc[td_actions].=se('del','group?id='.$tcm["auth_id"].'&amp;action=del_g'.$madd,'','');
	}
	if(empty($fmod)){
		$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];	
		$rc[td_name]=lng('Not in the group');
		$rc[td_users]='<a href="group?group=0">'.lng('Editing').'</a>';
	}
	$vars['post_html']=show_se();
	echo shell_tpl_admin('block/table',$vars);
	
	//=======================
	//  Формы добавления и редактирования группы
	//=======================
	$vars['path']='group';
	$is_edit=!empty($action) && $action=='edit_form' && isset($id) && check_group($id,'edit',0,$fmod);
	$is_add=empty($action) && check_group(0,'add',0,$fmod);
	if($is_edit || $is_add){
		if($is_edit){
			$vars['anchor']='edit_form';
			$vars['name']=lng('Edit group');
			$vars['icon']='edit';
			$vars['id']=$id;
			$vars['form_type']='edit';
			$vars['go_back_url']='group?id=0'.$madd;
			$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'edit');
			$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'id','value'=>$id);			
			getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id",1,'main_auth');
			foreach($db->Record AS $var=>$value){
				if($var=='session_lifetime') $var='user_session_lifetime';
				if($var=='session_multy') $var='user_session_multy';
				$$var=$value;
			}
		}
		if($is_add){
			$vars['anchor']='add_form';
			$vars['name']=lng('Add group');
			$vars['icon']='groupadd';
			$vars['id']=$id;
			$vars['form_type']='add';
			$vars['hidden']=1;
			$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'add');
			$group_name='';
			$group_sname='';
			$group_module=0;
			$user_session_lifetime=-1;
			$user_session_multy=-1;
		}
		$vars['section']['main']['fields'][]=Array('type'=>'static','content'=>$madd2);
			
		if($is_add) $vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'group_name','value'=>$group_name,'title'=>lng('Name'),'addon'=>'OnBlur="translate2(this,group_sname);"');
		else $vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'group_name','value'=>$group_name,'title'=>lng('Name'));
		
		$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'user_session_lifetime','value'=>$user_session_lifetime,'title'=>lng('Maximum session idle time (in minutes, 0 - infinity, -1 - use system defaults)'));
		$vars['section']['main']['fields'][]=Array('type'=>'select','name'=>'user_session_multy','value'=>$user_session_multy,'title'=>lng('Configure using multiple sessions (override system settings)'),'items'=>Array(
								Array('id'=>-1,'value'=>lng('use system settings')),
								Array('id'=>0,'value'=>lng('deny')),
								Array('id'=>1,'value'=>lng('allow'))
							));
		
		$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'group_sname','value'=>$group_sname,'title'=>lng('Unique special name in English'));
		$items=Array(
			Array('id'=>0, 'value'=>lng('No'))
		);
		global $modules;
		if(empty($modules)) $modules=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,"main_module");
		foreach($modules AS $m) if(check_mod($m["module_id"],'view') && (check_mod($m["module_id"],'edit') || $m["module_id"]==$group_module)){
			$items[]=Array('id'=>$m['module_id'],'value'=>$m['module_name']);
		}
		if(empty($group_module) && !empty($fmod) && $is_add) $group_module=$fmod;
		$vars['section']['main']['fields'][]=Array('type'=>'select','name'=>'group_module','value'=>$group_module,'title'=>lng('Belongs to the module'),'items'=>$items);		
		
		echo shell_tpl_admin('block/form', $vars);
	}

}

//=======================
//  Форма добавления/изменения юзера
//=======================

$is_edit=!empty($action) && $action=='edit' && isset($group) && !empty($id) && check_user(-$id,"edit",getrowval("SELECT auth_owner FROM main_auth WHERE auth_id=".$id,"auth_owner"),0,0,$fmod);
$is_add=empty($action) && check_user(0,'reg',0,0,0,$fmod) && (!isset($_GET["group"]) || (isset($_GET["group"]) && check_group($_GET["group"],'invite',0,$fmod)));
if($is_edit || $is_add){
	$vars['path']='group';
	if($is_edit){
		$vars['anchor']='edit';
		$vars['name']=lng('Edit user');
		$vars['icon']='edit2';
		$vars['id']=$id;		
		$vars['form_type']='edit';
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'edit2');
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'id','value'=>$id);
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'group','value'=>$group);		
		$vars['go_back_url']='group?group='.$group.$madd;
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=$id",1,"main_auth");
		foreach($db->Record AS $var=>$value) $$var=$value;
		$vars=add_user_form('','',$db->Record,$vars,1);
	}
	if($is_add){
		$vars['anchor']='add_user_form';
		$vars['name']=lng('Add user');
		$vars['icon']='useradd';
		$vars['form_type']='add';
		$vars['hidden']=1;
		$vars=add_user_form('','',Array(),$vars,1);
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'add2');
		$auth_owner=0;
	}
	$vars['section']['main']['fields'][]=Array('type'=>'static','content'=>$madd2);	
	if($is_edit){
		$lnks2=getall($db,"SELECT * FROM auth_link WHERE link_user=$id",1,"auth_link");
		$lnk=Array();
		foreach($lnks2 AS $lnk2) $lnk[$lnk2["link_group"]]=1;
	} else {
		$lnk=Array();
		if(isset($group)) $lnk[$group]=1;
	}
	if(!empty($tmm2)){
		$items=Array();
		$items[]=Array('id'=>'-100','value'=>'','addon'=>' style="display: none;" checked');
		foreach($tmm2 AS $mc)if(check_group($mc["auth_id"],'invite',0,$fmod) || isset($lnk[$mc["auth_id"]])){
			$items[]=Array('id'=>$mc['auth_id'],'value'=>$mc["group_name"]);
		}
	}
	if(!empty($items)) $vars['section']['main']['fields'][]=Array('type'=>'check_group','grand_name'=>'gbu','title'=>lng('Joined groups'),'items'=>$items,'value'=>$lnk);
	
	$items=Array();
	$add='';
	$chks=Array();
	if(!empty($auth_owner)){
		if(strstr($auth_owner,',')){
			$tmp=explode(',',$auth_owner);
			foreach($tmp AS $t) $chks[$t]=1;
		} else $chks[$auth_owner]=1;
	}
	$items[]=Array('id'=>'-1','value'=>lng('Super user'));
	$tos=getall($db,"SELECT * FROM main_auth WHERE auth_enable=1 AND auth_type=0 ORDER BY user_name");
	foreach($tos AS $t)if(check_user(-$t["auth_id"],'view',$t["auth_owner"],0,0,$fmod) || !empty($chk[$t["auth_id"]]) || $t["auth_id"]==$user->id){
		if(empty($auth_id) || $t["auth_id"]!=$auth_id){
			$items[]=Array('id'=>$t['auth_id'], 'value'=>$t['user_name'].' ('.$t['user_login'].')');
		}
	}
	if(!empty($items)){
		$vars['section']['additional']['hidden']=1;
		$vars['section']['additional']['title']=lng('Owners');
		$vars['section']['additional']['icon']='usrs';
		$vars['section']['additional']['hidden']=1;
		$vars['section']['additional']['fields'][]=Array('type'=>'check_group','grand_name'=>'u_owner','items'=>$items,'value'=>$chks);
	}
	
	echo shell_tpl_admin('block/form', $vars);
}

//=======================
//  Форма приглашения юзера
//=======================
if(isset($_GET["group"]) && $_GET["group"]!=0 && (empty($_GET['action']) || $_GET['action']!='edit') && check_group($_GET["group"],'invite',0,$fmod)){
		$s=select_users();
		if(!empty($s)){
			$vars['anchor']='ivite_user_form';
			$vars['name']=lng('Invite user');
			$vars['icon']='invite';
			$vars['form_type']='add';
			$vars['hidden']=1;
			$vars['path']='group?group='.$_GET["group"];
			$vars['btn_title']=lng('Invite');
			$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'invite');
			$vars['section']['main']['fields'][]=Array('type'=>'static','content'=>$madd2.'<p>'.lng('Select user').': <br><select name="auth_id">'.$s.'</select></p>');
			echo shell_tpl_admin('block/form', $vars);
		}	
}

?>