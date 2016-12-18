<?php

ob_start();
$use_crosstables=true;
$use_titles=true;
include('afunc.inc');		//функции админки

//рутинная работа
start_auto_index();

//делаем пост и гет глобальными
//foreach($_POST AS $var=>$value) $$var=$value;
//foreach($_GET AS $var=>$value) $$var=$value;

// Даём понять, что JQuery мы в любом случае используем в backend
$GLOBALS['g_par']['use_jquery']=1;

// Разрешаем некоторые глобальные переменные
stf('globals');
//поддержка nginx+fpm
if(isset($id) && !isset($_GET['id']) && !isset($_POST['id'])) $_GET['id']=$id;
//all
get_globals(Array('sid','id','id2','id3','id4','id5','id6','id7','id8','action','smb1','type','section','x','y','z','a','b','c','submit','row','col'));
//mod_table
get_globals(Array('ncm','ncz','page','sort','f','data','ro_owner','ro_owner2','chk_type','new_own','new_own2','new_ex','selcol','part','uc','wh','sbt','chk_type','fe','pos','chk','ar','r','bfe','size','new_ownB','selbuff'));
//mod_col
get_globals(Array('col_name','col_hint','col_sname','col_inform','col_cat','col_default','col_type','file_dirB','file_totalmaxB','file_typesB','col_unique2','col_order2','col_order3','col_paramlink','col_part','col_deflist','col_link','col_speclink','col_link3','col_deep','col_link2','col_link4','col_filterB','file_dir','file_maxsize','file_totalmax','file_prefix','file_types','file_genname','module_type','col_onform','col_onshow','col_oninsert','col_bold','col_required','col_fastedit','col_unique','col_url','col_filterA','col_index','col_filterC','module_url','col_linkG','p','use_tpl','col_tpl','upload','install_components'));
//mod_main
get_globals(Array('table_name','table_sname','table_tpl','table_extype','table_onedit','table_top','table_bottom','table_multy','table_cansub','sub_id','table_icon','table_public','table_slice')); //tables
get_globals(Array('ex_name','ex_sname','new_zone','ex_major','ex_public','ex_upload')); //exs, also it's used in mod_table
get_globals(Array('part_name','part_sname','part_parse','part_type','part_pic','part_table','part_access','part_url','part_ifrow','part_owner','part_ex','part_cur','part_file','part_ifcase','part_folder','file','part_auth','part_unsafe','part_ignore','part_404','part_ifdetect','part_iowner','part_sowner','part_skipurl'));//parts
get_globals(Array('timer_type','timer_date','timer_time','timer_x','timer_y'));//timers
get_globals(Array('param_name','param_sname','param_default','param_type','param_list','param_link','param_get','param_array','part_id','param_hide'));//part_param
get_globals(Array('part_body','body'));
get_globals(Array('query'));
//terminal
get_globals(Array('lang','code_sql','code_php','user_e5','code_e5','code_terminal'));
//statistics
get_globals(Array('period','select_type','select_month','select_year','select_day','source','select_object'));
//perms
get_globals(Array('fmod','group_name','group_sname','group_module','group','spec','collect','auth_folder','auth_folder2'));
get_globals(Array('user_login','user_pwl','user_pwl2','user_pwlcode','user_email','user_name','user_fixedip','auth_id','gbu','user_session_lifetime','user_session_multy'));
//mail
get_globals(Array('set_type','view','del','mail_to','mail_topic','mail_body'));
//zone
get_globals(Array('addwww','zone_folder','zone_redirect','zone_iprange','zone_robots','zone_tpl','zone_safe','zone_autosub','zone_email','ex_zone','zone_name','zone_domain','zone_module','newex_name','newex_sname'));
//modules
get_globals(Array('module_name','module_sname','module_major','module_icon','module_public_ex'));
//components
get_globals(Array('cat_name','cat_pre','cat_after','part_cat','cat_id','part_about'));//else array of post names is in mod_main->parts zone
//update
get_globals(Array('upd_srv','up','pg','m2','p'));
//settings
get_globals(Array('off','on','clean','new_ip','new_email','is_offline2','do_backup2','do_backup_files2','cron_type2','cron_su2','correct_utf2','check_for_xss2','protect_admin_form2','use_ace2','server2','username2','password2','database2','restore_db','use_dump','su_login','su_pwl','system_email2','def_chmod2','def_drmod2','def_charset2','cron_jobs2','no_update2','black_ips2','rewrite_upload_max_filesize2','rewrite_post_max_size2','rewrite_memory_limit2','rewrite_max_execution_time2','rewrite_max_input_time2'));
get_globals(Array('statistics2','del_hour_history2','del_day_history2','del_ip_history2','del_source_history2','collect_object_history2','collect_sources_history2','ignore_bot2','show_404_2','session_multy2','session_storage2','session_lifetime2','session_everytime2','clear_db','use_dump2','superuser_ip2','max_cache_size_mb2'));
//deploy
get_globals(Array('del_dump','date','build','build2','build3','url','ftp_server','ftp_login','ftp_password','ftp_folder','mysql_server','mysql_login','mysql_password','mysql_database'));
etf('globals');

//делаем нормализацию URL для админки
global $base_dir;
$module=str_replace($zone[$za]["zone_folder"].'/','',$_SERVER["REQUEST_URI"]);
if(!empty($module) && $module[0]=='/') $module=substr($module,1);
if(strpos($module,'?')){
	$module=explode('?',$module);
	parse_str($module[1]);
	$module=$module[0];
}

$inc=false;
if(!empty($module) && file_exists(DOCUMENT_ROOT.'/core/editor/'.$module.'.php')) $inc=true;
if(!empty($module) && $module!='/' && file_exists(DOCUMENT_ROOT.'/core/editor/'.$module)) $inc=true;
if(!empty($module) && strpos($module,'/')){
	$left_url=explode('/',$module);
	$module=array_shift($left_url);
	$inc=true;
}
if(!$inc) $module='main';

//подготовка заголовков
header('Content-type: text/html; charset="utf-8"');
header('Pragma: no-cache');
header('Cache-Control: no-cache, must-revalidate');
ob_start();

//подготовка переменных
if($module!='ajax'){
	create_bread();
	$mail_cnt=seek_mail_count();
	$use_crosstables=false;
	$use_titles=false;
	if($mail_cnt==0) $mail_cnt='';
	if(!empty($user->login)) $ulogin=$user->login;
	if(empty($user->login)) $ulogin='Guest';
	if(strlen($ulogin)==1) $ulogin=$ulogin.' ';
	if($user->super) $ulogin='SuperUser';
	$user_url='';
	if($user->super) $user_url=$zone_url.'/settings#su';
	else {
		if(check_user(-$user->id,'edit')) $user_url=$zone_url.'/group?group=0&id='.$user->id.'&action=edit#edit';
	}
	$genter=0;
	if($user->super) $genter=1;
	if(check_user(0,'reg')) $genter=1;
	if(!$genter){
		global $db;
		$gps=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_owner=0",1,'main_auth');
		if(!empty($gps)) foreach($gps AS $g) if(check_group($g["auth_id"],'view')){$genter=1; break;}
	}
	$senter=0;
	if($user->super) $senter=1;
	if(check_zone(0,'rules')) $senter=1;
	if(!$senter){
		foreach($zone AS $z) if(check_zone($z["zone_id"],'rules')){$senter=1; break;}
	}
	$GLOBALS["genter"]=$genter;
	$GLOBALS["senter"]=$senter;
	$GLOBALS['mail_cnt']=$mail_cnt;
	$GLOBALS['mail_url']=$zone_url.'/mail';
	$GLOBALS['ulogin']=$ulogin;
	$GLOBALS['first_uname']=substr5($ulogin,0,1);
	$GLOBALS['last_uname']=substr5($ulogin,1);
	$GLOBALS['user_url']=$user_url;
	$GLOBALS['view_zone']=(check_zone(0,'enter'));
}

//вывод основного контента в переменную
global $main_content;
$ctmp=start_buffer();
$inc=false;
if(!empty($module) && file_exists(DOCUMENT_ROOT.'/core/editor/'.$module.'.php')){
	include(DOCUMENT_ROOT.'/core/editor/'.$module.'.php');
	$inc=true;
}
if(!empty($module) && $module!='/' && file_exists(DOCUMENT_ROOT.'/core/editor/'.$module) && is_file(DOCUMENT_ROOT.'/core/editor/'.$module)){
	include(DOCUMENT_ROOT.'/core/editor/'.$module);
	$inc=true;
}
if(!$inc) include(DOCUMENT_ROOT.'/core/editor/main.php');
$main_content=return_buffer($ctmp);

//запуск шаблона, либо вывод основного контента
if($module!='ajax'){
	echo shell_tpl_admin(DOCUMENT_ROOT.'/core/editor/tpl/admin.tpl');
} else echo $main_content;

ob_end_flush();

?>