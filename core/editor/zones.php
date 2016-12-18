<?php

define_lng('zones'); // преимущественно будет использоваться перевод из секции zones

//=======================
//  Добавление
//=======================
if(!empty($action) && $action=='add' && check_zone(0,'add')){
	$prefix='new';
	$zone_safe=0;
	if(strstr($zone_module,'n') && isset($_POST[$prefix."ex_name"])){
		$module=str_replace('n','',$zone_module);
		$new_ex=add_ex($module,$_POST[$prefix."ex_name"],$_POST[$prefix."ex_sname"],$_POST[$prefix."ex_major"],0,$prefix);
		if($new_ex){
			$zone_module=$module.':'.$new_ex;
		} else $zone_module=-3;
	}
	if(strstr($zone_module,'n')) $zone_module=-3;

	$db->query("INSERT INTO main_zone (zone_domain, zone_folder, zone_redirect, zone_module, zone_name, zone_iprange, zone_robots, zone_email, zone_tpl, zone_safe, zone_autosub)
			VALUES ('$zone_domain', '$zone_folder', $zone_redirect, '$zone_module', '$zone_name', '$zone_iprange', '$zone_robots', '$zone_email','$zone_tpl', $zone_safe, $zone_autosub)",3,'main_zone');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	
	$vars['title']=lng('Site successfully added');
	
	if(strstr($zone_module,':')){
		$zm=explode(':',$zone_module);
		$zna=$zm;
		$ze=$zm[1];
		$zm=$zm[0];
		$add=true;
		if(!empty($ex_zone)) foreach($ex_zone AS $var=>$value) if(!empty($value) && !empty($var) && $value==$ze && $var==$zm){$add=false; break;}
		if($add){
			$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
				VALUES($sid,$ze,$zm)",3,"ex_zone");
		}		
		
		if(empty($name_cache[$zna[0]])) $name_cache[$zna[0]]=getrowval("SELECT * FROM main_module WHERE module_id=".$zna[0],"module_name");
		$mname=$name_cache[$zna[0]];
		getrow($db,"SELECT * FROM ex_module WHERE ex_id=".$zna[1]);
		$ename=$db->Record["ex_name"];
		$etable=$db->Record["ex_major"];
		if(empty($etable)) $etable=getrowval('SELECT table_id FROM main_table WHERE table_module='.$zna[0].' AND table_bold=1','table_id');
		if(!empty($etable)){
			$etable=getrowval("SELECT * FROM main_table WHERE table_module=".$zna[0]." AND table_bold=1","table_id");
			$extable=getrowval("SELECT * FROM ex_group WHERE ex_table=$etable AND ex_ex2=".$zna[1],"ex_ex1");
			$vars['msg']='<a href="'.$zone_url.'/mod_table?id='.$zna[0].'&amp;id2='.$extable.'&amp;action=setcz&amp;ncz='.$sid.'&amp;ncm='.$zna[1].'">'.lng('Go to management').' ('.$mname.' / '.$ename.')</a>';
		} else {
			$vars['msg']='<a href="'.$zone_url.'/mod_main?id='.$zna[0].'">'.lng('Go to management').' ('.$mname.')</a>';
		}
		
	}
	
	if(!empty($addwww) && empty($zone_redirect)){
		$db->query("INSERT INTO main_zone (zone_domain, zone_folder, zone_redirect, zone_module, zone_name, zone_iprange, zone_robots, zone_email, zone_tpl)
				VALUES ('www.".$zone_domain."', '$zone_folder', $sid, '$zone_module', '$zone_name', '$zone_iprange', '$zone_robots', '$zone_email', '$zone_tpl')",3,'main_zone');
	}

	if(!empty($ex_zone)) foreach($ex_zone AS $var=>$value)if(!empty($value) && !empty($var)){
		$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
			VALUES($sid,$value,$var)",3,"ex_zone");
	}
	$action='';
	
	echo shell_tpl_admin('block/message_box', $vars);
}

//=======================
//  Удаление
//=======================
if(!empty($action) && $action=='del' && !empty($id) && check_zone(0,'del') && check_form_protection_key($_GET['key'],'zones',1)){
	getrow($db,"SELECT * FROM main_zone WHERE zone_id=$id",1,'main_zone');
	$protect=false;
	$vars['title']=lng('Deleting a site');
	if($db->Record["zone_module"]==-1){
		$zx=getall($db,"SELECT * FROM main_zone WHERE zone_module=-1 AND zone_active=1 AND zone_redirect=0",1,'main_zone');
		if(count($zx)==1){
			$vars['msg']=lng('This is the last active area with the administering part. Unable to remove.');
			$protect=true;
		}
	}
	if(!$protect){
		$vars['msg']=lng('Area successfully removed');
		$db->query("DELETE FROM main_zone WHERE zone_id=$id",3,'main_zone');
		$db->query("DELETE FROM main_zone WHERE zone_redirect=$id",3,'main_zone');
		$db->query("DELETE FROM ex_zone WHERE ex_zone=$id",3,'ex_zone');
	}
	echo shell_tpl_admin('block/message_box', $vars);
	$action='';
}

//=======================
//  Редактирование
//=======================
if(!empty($action) && $action=='edit' && isset($id) && check_zone($id,'edit')){
	getrow($db,"SELECT * FROM main_zone WHERE zone_id=$id",1,'main_zone');
	$zrd=$db->Record["zone_redirect"];
	$zmd=$db->Record["zone_module"];
	$protect=false;
	
	$vars['title']=lng('Edit site');
	if($db->Record["zone_module"]==-1){
		$zx=getall($db,"SELECT * FROM main_zone WHERE zone_active=1 AND zone_module=-1 AND zone_redirect=0",1,'main_zone');
		if(count($zx)==1 && $zone_module!='-1'){
			$vars['msg']=lng('This is the last active area with the administering part. Unable to change module.');
			$protect=true;
		}
	}
	if(!$protect) {
		$vars['msg']=lng('Site updated successfully');
		$prefix='new';
		$zone_safe=0;
		$dont_add_twice=false;
		if(strstr($zone_module,'n') && isset($_POST[$prefix."ex_name"])){
			$module=str_replace('n','',$zone_module);
			$new_ex=add_ex($module,$_POST[$prefix."ex_name"],$_POST[$prefix."ex_sname"],$_POST[$prefix."ex_major"],0,$prefix);
			if($new_ex){
				$zone_module=$module.':'.$new_ex;
				$dont_add_twice=true;								
				$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
					VALUES($id,$new_ex,$module)",3,"ex_zone");
			} else $zone_module=-3;
		}
		if(strstr($zone_module,'n')) $zone_module=-3;		
	
		if($zmd!=$zone_module && strstr($zone_module,':')){
			$sid=$id;
			$zm=explode(':',$zone_module);
			$ze=$zm[1];
			$zm=$zm[0];
			$add=true;
			getrow($db,"SELECT * FROM ex_zone WHERE ex_zone=$sid AND ex_module=$ze AND ex_module2=$zm");
			if(!empty($db->Record)){
				if(!empty($ex_zone)) foreach($ex_zone AS $var=>$value) if(!empty($value) && !empty($var) && $value==$ze && $var==$zm){$add=false; break;}
				if($add && !$dont_add_twice){
					$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
						VALUES($sid,$ze,$zm)",3,"ex_zone");
				}
			}
		}
		if($zrd!=$zone_redirect && $zone_redirect!=0) $db->query("UPDATE main_zone SET zone_redirect=$zone_redirect WHERE zone_redirect=$id",3,'main_zone');
		$db->query("UPDATE main_zone SET
			zone_name='$zone_name',
			zone_domain='$zone_domain',
			zone_folder='$zone_folder',
			zone_redirect=$zone_redirect,
			zone_iprange='$zone_iprange',
			zone_module='$zone_module',
			zone_robots='$zone_robots',
			zone_tpl='$zone_tpl',
			zone_safe=$zone_safe,
			zone_email='$zone_email',
			zone_autosub=$zone_autosub
		WHERE zone_id=$id",3,'main_zone');
	}
	echo shell_tpl_admin('block/message_box', $vars);
	$action='';
}

//=======================
//  Активировать
//=======================
if(!empty($action) && $action=='activate' && !empty($id) && check_zone($id,'edit') && check_form_protection_key($_GET['key'],'zones',1)){
	$db->query("UPDATE main_zone SET
		zone_active=1
	WHERE zone_id=$id",3,'main_zone');
	$vars['title']=lng('Site activated');
	echo shell_tpl_admin('block/message_box', $vars);
	$action='';
}

//=======================
//  Деактивировать
//=======================
if(!empty($action) && $action=='deactivate' && !empty($id) && check_zone($id,'edit') && check_form_protection_key($_GET['key'],'zones',1)){
	getrow($db,"SELECT * FROM main_zone WHERE zone_id=$id",1,'main_zone');
	$protect=false;
	$vars['title']=lng('Site deactivated');
	if($db->Record["zone_module"]==-1){
		$zx=getall($db,"SELECT * FROM main_zone WHERE zone_active=1 AND zone_module=-1 AND zone_redirect=0",1,'main_zone');
		if(count($zx)==1){
			$vars['title']=lng('Site can\'t be deactivated');
			$vars['msg']=lng('This is the last active zone with the administering part. Unable to deactivate.');
			$protect=true;
		}
	}
	if(!$protect) $db->query("UPDATE main_zone SET zone_active=0 WHERE zone_id=$id",3,'main_zone');
	echo shell_tpl_admin('block/message_box', $vars);
	$action='';
}

//=======================
//  Вывод таблицы зон
//=======================

$vars['th']=Array(lng('Name'),lng('Module'),lng('Domain'),lng('Folder'),lng('Actions'));
sdefine('td_name',0);
sdefine('td_module',1);
sdefine('td_domain',2);
sdefine('td_folder',3);
sdefine('td_actions',4);
$vars['rows']=Array();

$tmp=getall($db,"SELECT * FROM main_zone ORDER BY zone_redirect, zone_domain, zone_folder",1,'main_zone');
$m=Array();
if(!empty($tmp)) foreach($tmp AS $tm){
	if($tm["zone_redirect"]==0) $m[$tm["zone_id"]]->zone=$tm;
	else {
		$rd=$tm["zone_redirect"];
		if($rd<0) $rd=-$rd;
		if(empty($m[$rd]->szone)) $m[$rd]->szone=Array();
		$m[$rd]->szone[]=$tm;
	}
}
if(!empty($m)){
	$tm=$m;
	$m=Array();
	foreach($tm AS $tmp){
		$m[str_replace('www.','',$tmp->zone["zone_domain"])][$tmp->zone["zone_id"]]=$tmp;
	}
}
if(!empty($m)){
	$vars['pre_html']='<br>';
	ksort($m);
	foreach($m AS $tmp)foreach($tmp AS $tcm) if(check_zone($tcm->zone["zone_id"],'edit')){
		$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];
		$cm=$tcm->zone;
		if(empty($tcm->szone)) $rc[td_name]=$cm["zone_name"];
		else {
			$tv=Array('id'=>$cm['zone_id'],'name'=>$cm['zone_name']);
			$rc[td_name]=shell_tpl_admin('block/tree_expand',$tv);
		}
		if($cm["zone_safe"]!=0) $rc[td_name].=si('protect');
		
		if($cm["zone_module"]==-3) $rc[td_module]='Нет';
		else if($cm["zone_module"]==-2) $rc[td_module]=lng('Update server');
		else if($cm["zone_module"]==-1) $rc[td_module]=lng('Admin cabinet');
		else if(strstr($cm["zone_module"],':')){
			$zna=explode(':',$cm["zone_module"]);
			if(empty($name_cache[$zna[0]])) $name_cache[$zna[0]]=getrowval("SELECT * FROM main_module WHERE module_id=".$zna[0],"module_name");
			$mname=$name_cache[$zna[0]];
			getrow($db,"SELECT * FROM ex_module WHERE ex_id=".$zna[1]);
			$ename=$db->Record["ex_name"];
			$etable=$db->Record["ex_major"];
			if(empty($etable)) $etable=getrowval("SELECT * FROM main_table WHERE table_module=".$zna[0]." AND table_bold=1","table_id");
			if(!empty($etable)){
				$extable=getrowval("SELECT * FROM ex_group WHERE ex_table=$etable AND ex_ex2=".$zna[1],"ex_ex1");
				$rc[td_module]= '<a href="'.$zone_url.'/mod_table?id='.$zna[0].'&amp;id2='.$extable.'&amp;action=setcz&amp;ncz='.$cm["zone_id"].'&amp;ncm='.$zna[1].'">'.$mname.' ('.$ename.')</a>';
			} else {
				if(!empty($mname) || !empty($ename)) $rc[td_module]=$mname.' ('.$ename.')';
				else $rc[td_module]=lng('Module is not available');
			}
		}
		
		if(empty($cm["zone_domain"])) $cm["zone_domain"]='*';
		$rc[td_domain]=$cm['zone_domain'];
		$rc[td_folder]=$cm['zone_folder'];
		if($cm["zone_active"]==1) $rc[td_actions]=se('deactivate','zones?id='.$cm["zone_id"].'&amp;action=deactivate&amp;key='.get_form_protection_key('zones',1,0));
		else $rc[td_actions]=se('activate','zones?id='.$cm["zone_id"].'&amp;action=activate&amp;key='.get_form_protection_key('zones',1,0));
		$rc[td_actions].=se('edit','zones?id='.$cm["zone_id"].'&amp;action=edit_form#form');
		if(check_zone(0,'del')) $rc[td_actions].=se('del','zones?id='.$cm["zone_id"].'&amp;action=del&amp;key='.get_form_protection_key('zones',1,0),'',' onclick="return(confirm(\''.lng('Are you sure?').'\'))"');
		if(!empty($tcm->szone)){
			$r=&$vars['rows'][];
			$r['id']='hpanel'.$cm['zone_id'];
			$r['style']='display: none;';
			$r['subtable']=Array();
			foreach($tcm->szone AS $cm){
				$subr=&$r['subtable'][];
				$rc=&$subr['cols'];
				$rc[td_name]=$cm["zone_name"];
				$rc[td_module]='';
				if(empty($cm["zone_domain"])) $cm["zone_domain"]='*';
				$rc[td_domain]=$cm['zone_domain'];
				$rc[td_folder]=$cm['zone_folder'];
				if($cm["zone_active"]==1) $rc[td_actions]=se('deactivate','zones?id='.$cm["zone_id"].'&amp;action=deactivate&amp;key='.get_form_protection_key('zones',1,0));
				else $rc[td_actions]=se('activate','zones?id='.$cm["zone_id"].'&amp;action=activate&amp;key='.get_form_protection_key('zones',1,0));
				$rc[td_actions].=se('edit','zones?id='.$cm["zone_id"].'&amp;action=edit_form#form');
				if(check_zone(0,'del')) $rc[td_actions].=se('del','zones?id='.$cm["zone_id"].'&amp;action=del&amp;key='.get_form_protection_key('zones',1,0).'"','',' onclick="return(confirm(\''.lng('Are you sure?').'\'))"');
			}
		}
	}
}

$vars['post_html']=show_se();
echo shell_tpl_admin('block/table',$vars);

//=======================
//  Форма редактирования / добавления
//=======================
$is_edit=!empty($action) && $action=='edit_form' && isset($id) && check_zone($id,'edit');
$is_add=empty($action)  && check_zone(0,'add');
if($is_edit || $is_add){
	if($is_add){
		$vars['anchor']='add_form';
		$vars['name']=lng('Add site');
		$vars['icon']='add';
		$vars['id']=0;
		$vars['form_type']='add';
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'add');
		//default values
		$zone_id=0;
		$zone_name='';
		$zone_domain='';
		$zone_module=0;
		$zone_folder='';
		$zone_redirect=0;
		$zone_iprange='';
		$zone_robots='';
		$zone_tpl='';
		$zone_safe=0;
		$zone_autosub=0;
		$zone_email='';
	}
	if($is_edit){
		$vars['anchor']='form';
		$vars['name']=lng('Edit site');
		$vars['icon']='edit';
		$vars['id']=$id;
		$vars['form_type']='edit';
		getrow($db,"SELECT * FROM main_zone WHERE zone_id=$id",1,'main_zone');
		foreach($db->Record AS $var=>$value)$$var=$value;
		$vars['zone_module']=$zone_module;
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'edit');
		$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'id','value'=>$id);
	}
	$vars['path']='zones';
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'zone_name','value'=>$zone_name,'title'=>lng('Name'));
	$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>'zone_domain','value'=>$zone_domain,'title'=>lng('Domain'));
	$vars['section']['main']['fields'][]=Array('type'=>'component','component'=>'editor.zones.module_selector','name'=>'zone_module','title'=>lng('Base module'));
	
	$vars['section']['additional']['title']=lng('Additional');
	$vars['section']['additional']['icon']='point';
	$vars['section']['additional']['hidden']=1;
	$vars['section']['additional']['fields'][]=Array('type'=>'text','name'=>'zone_folder','value'=>$zone_folder,'title'=>lng('Folder'));

	$items=Array();
	$items[]=Array('id'=>0, 'value'=>lng('Not defined'));	
	$zones=getall($db,"SELECT * FROM main_zone WHERE zone_id!=$zone_id ORDER BY zone_domain",1,'main_zone');
	if(!empty($zones)) foreach($zones AS $zone2)if($zone2["zone_redirect"]==0){
		$items[]=Array('id'=>$zone2["zone_id"], 'value'=>$zone2["zone_domain"].' '.$zone2["zone_folder"].' ('.lng('clone').')');
		$items[]=Array('id'=>'-'.$zone2["zone_id"], 'value'=>$zone2["zone_domain"].' '.$zone2["zone_folder"].' ('.lng('301 redirect').')');
	}
	$vars['section']['additional']['fields'][]=Array('type'=>'select','name'=>'zone_redirect','value'=>$zone_redirect,'title'=>lng('Redirect'),'items'=>$items);

	$vars['section']['additional']['fields'][]=Array('type'=>'textarea','name'=>'zone_iprange','value'=>$zone_iprange,'title'=>lng('IP-configuration'));
	$vars['section']['additional']['fields'][]=Array('type'=>'textarea','name'=>'zone_robots','value'=>$zone_robots,'title'=>lng('Robots.txt file'));
	$vars['section']['additional']['fields'][]=Array('type'=>'textarea','name'=>'zone_tpl','value'=>$zone_tpl,'title'=>lng('Shell template'));

	//$items=Array();
	//$items[]=Array('id'=>0, 'value'=>lng('No'));
	//$items[]=Array('id'=>1, 'value'=>lng('Protect password'));
	//$items[]=Array('id'=>2, 'value'=>lng('Protect content and password'));
	//$vars['section']['additional']['fields'][]=Array('type'=>'select','name'=>'zone_safe','value'=>$zone_safe,'title'=>lng('Data protect (for private area only)'),'items'=>$items);

	$items=Array();
	$items[]=Array('id'=>0, 'value'=>lng('No'));
	$items[]=Array('id'=>1, 'value'=>lng('Turn subdomains to URL postfix'));
	$items[]=Array('id'=>2, 'value'=>lng('Turn subdomains to SUBDOMAIN variable'));
	$vars['section']['additional']['fields'][]=Array('type'=>'select','name'=>'zone_autosub','value'=>$zone_autosub,'title'=>lng('Automatic subdomains'),'items'=>$items);
	
	$vars['section']['additional']['fields'][]=Array('type'=>'text','name'=>'zone_email','value'=>$zone_email,'title'=>lng('Admin email'));

	if($is_add){
		$vars['section']['main']['fields'][]=Array('type'=>'checkbox','name'=>'addwww','value'=>0,'caption'=>lng('Add WWW subdomain'));
		
		$vars['section']['modules']['title']=lng('Integration with modules');
		$vars['section']['modules']['icon']='link';
		$vars['section']['modules']['hidden']=1;
		$vars['section']['modules']['fields'][]=Array('type'=>'component','component'=>'editor.zones.misc_module_selector');
	}
	
	echo shell_tpl_admin('block/form', $vars);

}

?>