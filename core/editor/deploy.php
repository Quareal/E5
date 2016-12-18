<?php

ini_set('max_execution_time',400);
define_lng('deploy');
if(is_array($url)) $url='';
$ok=false;
global $user; if($user->super==0) {include('main.php'); exit;}
if(!empty($_POST["action"])) $action=$_POST["action"];
if(!empty($_GET["action"])) $action=$_GET["action"];

function test_ftp(&$ftp,$dir,$file,$www,$step=0,$seek=1){
	$fls=f_nlist($ftp,$dir);
	//if(empty($fls)) return -1;
	$fldrs=Array();
	$i=0;
	if(!empty($fls)) foreach($fls AS $f){
		if(!empty($dir)) $ndir=$dir.'/'.$f; else $ndir=$f;
		$fldrs[$i]->name=$f;
		$fldrs[$i]->f=Array();
		$fls2=f_nlist($ftp,$ndir);
		foreach($fls2 AS $f2) $fldrs[$i]->f[]=$f2;
		$i++;
	}

	if(f_put($ftp,'e5starter.php',$file, FTP_ASCII)){
		if(@file_get_contents($www.'e5starter.php')){
			f_delete($ftp,'e5starter.php');
			return '';
		}
		f_delete($ftp,'e5starter.php');
	}
	foreach($fldrs AS $f){
		if(f_put($ftp,$f->name.'/e5starter.php',$file, FTP_ASCII)){
			if(@file_get_contents($www.'e5starter.php')){
				f_delete($ftp,$f->name.'/e5starter.php');
				return $f->name;
			}
			f_delete($ftp,$f->name.'/e5starter.php');
		}
	}
	foreach($fldrs AS $f) foreach($f->f AS $f2){
		if(f_put($ftp,$f->name.'/'.$f2.'/e5starter.php',$file, FTP_ASCII)){
			if(@file_get_contents($www.'e5starter.php')){
				f_delete($ftp,$f->name.'/'.$f2.'/e5starter.php');
				return $f->name.'/'.$f2;
			}
			f_delete($ftp,$f->name.'/'.$f2.'/e5starter.php');
		}
	}
	return -1;
}

if(!empty($action) && $action=='install'){
	//попытка подключения к FTP серверу
	$ok=false;
	$GLOBALS["allow_ftp"]=1;
	@$f=f_connect($ftp_server);
	@$b=f_login($f,$ftp_login,$ftp_password);
	$vars=Array();
	if(!$b){
		$vars['title']=lng('FTP server settings are incorrect');
	} else {
		$tmp=DOCUMENT_ROOT.'/core/editor/install_tmp.php';
		//определение корневой папки для домена
		$d=fopen($tmp,'w');	fwrite($d,'1');fclose($d);
		if($url[strlen($url)-1]!='/') $url.='/';
		if(!strstr($url,'http')) $url='http://'.$url;
		$ftp_folder=test_ftp($f,'',$tmp,$url,0,1);
		if($ftp_folder==-1){
			$GLOBALS["allow_ftp"]=2;
			@$f=f_connect($ftp_server);
			@$b=f_login($f,$ftp_login,$ftp_password);
			$ftp_folder=test_ftp($f,'',$tmp,$url,0,1);
		}
		if($ftp_folder!=-1){
			$ftp_ofolder=$ftp_folder;
			$ftp_folder.='/';
			//проверка MySQL реквизитов
			$tmp=DOCUMENT_ROOT.'/core/editor/install_tmp.php';
			$d=fopen($tmp,'w');
			fwrite($d,'<?php
				echo " OK";
				$t = mysql_connect("'.$mysql_server.'", "'.$mysql_login.'", "'.$mysql_password.'");
				if(!$t) echo "ERR@1";
				if(!mysql_select_db("'.$mysql_database.'",$t)) echo "ERR@2";
			                ?>');
			fclose($d);
			if(!@f_put($f,$ftp_folder.'test.php',$tmp, FTP_ASCII)){
				$ftp_folder=$ftp_ofolder;
			}
			if(@f_put($f,$ftp_folder.'test.php',$tmp, FTP_ASCII)){
				@$str=implode('',file($url.'test.php'));
				//sleep(2);
				@f_delete($f,$ftp_folder.'test.php');
				if(!strpos($str,'OK')){
					$ftp_folder=$ftp_ofolder;
					if(@f_put($f,$ftp_folder.'test.php',$tmp, FTP_ASCII)){
						@$str=implode('',file($url.'test.php'));
						@f_delete($f,$ftp_folder.'test.php');
					}
				}
				if(!strpos($str,'OK')){
					//echo '<div><b style="color:#FF0000;">Не удаётся найти корневую папку для указанного домена</b>. Воспользуйтесь ручной установкой системы</div>';
					$vars['title']=lng('Unable to find the root folder for the specified domain');
					$vars['msg']=lng('Use manual installation of the system');
				} else if(strpos($str,'ERR@1')){
					//echo '<div><b style="color:#FF0000;">Неправильно указан сервер, либо логин, либо пароль доступа к MySQL серверу</b></div>';
					$vars['title']=lng('Error connection to MySQL server');
					$vars['msg']=lng('Access details are incorrect, or the server does not support remote access');
				} else if(strpos($str,'ERR@2')){
					//echo '<div><b style="color:#FF0000;">На сервере MySQL не существует указанной базы данных</b></div>';
					$vars['title']=lng('The MySQL server does not have specified database');
				}else {
					$ok=true;
				}
			} else {
				//echo '<div><b style="color:#FF0000;">Не удаётся загрузить файлы на сервер. Проверьте настройки доступа и правильность пути</b></div>';
				$vars['title']=lng('Unable to upload files to the server');
				$vars['msg']=lng('Check the settings of the access and correctness of path');
			}
		} else {
			//echo '<div><b style="color:#FF0000;">Не удаётся найти корневую папку для указанного домена</b>. Воспользуйтесь ручной установкой системы</div>';
			$vars['title']=lng('Unable to find the root folder for the specified domain');
			$vars['msg']=lng('Use manual installation of the system');
		}
		if(file_exists($tmp)) unlink($tmp);
	}
	if(!empty($vars)){
		$vars['type']='error';
		echo shell_tpl_admin('block/message_box',$vars);
	}
	f_close($f);
	if($ok) $action='install2';
}

if(!empty($action) && $action=='install2'){

	global $update_server,$database,$server,$username,$password;
	global $db;
	getrow($db,"SELECT * FROM main_zone WHERE zone_module=-2",1,"main_zone");
	if(empty($db->Record)){
		echo '<p><b style="color:#FF0000;">'.lng('For a successful installation, you must create a area for update server').'</b></p>';
	} else {
		$upd='http://'.$_SERVER["HTTP_HOST2"].'/'.$db->Record["zone_folder"];

		$f=f_connect($ftp_server);
		f_login($f,$ftp_login,$ftp_password);
		@f_chmod($f,DEF_DRMOD,substr($ftp_folder,0,strlen($ftp_folder)-1));
		@f_delete($f,$ftp_folder.'.htaccess');
		@f_delete($f,$ftp_folder.'index.php');
		@f_delete($f,$ftp_folder.'core/.htaccess');
		@f_delete($f,$ftp_folder.'index.html');
		
		$tt=gzip_folders($GLOBALS["UPDATE_PATHS"],Array('config.inc','timing.inc'),Array('files/pub'),1,0,'',0,0,1,1,1);		
		//$tt=gzip_folders(Array('','core','core/install','core/update','core/units','core/editor', 'core/editor/tpl', 'core/units/template', 'core/units/left','files/editor','files/editor/classic','files/editor/icons','files/editor/master'),Array('config.inc'),Array('files/pub'),1,0,'',0,0,1,1);		
		$b=fopen(DOCUMENT_ROOT.'/core/editor/temp.php','w');
		fwrite($b,$tt);fclose($b);
		f_put($f,$ftp_folder.'temp.php',DOCUMENT_ROOT.'/core/editor/temp.php', FTP_BINARY);
		//@f_chmod($f,0777,$ftp_folder.'temp.php');	
		$tmp=file($url.'temp.php');		

		$f2=fopen(DOCUMENT_ROOT.'/core/editor/temp.php','w');
		fwrite($f2,"<?php
\$database='$mysql_database';
\$server='$mysql_server';
\$username='$mysql_login';
\$password='$mysql_password';
\$update_server='$upd';
\$def_chmod='0777';
\$def_drmod='0777';
?>");	
		fclose($f2);
		f_put($f,$ftp_folder.'core/config.inc',DOCUMENT_ROOT.'/core/editor/temp.php', FTP_BINARY);
		@f_chmod($f,DEF_CHMOD,$ftp_folder.'core/config.inc');
		unlink(DOCUMENT_ROOT.'/core/editor/temp.php');
		
		f_close($f);	
		$tmp=@file($url.'core/editor/update.php?action=update&inst=1&remote=2');
		if(!$tmp){
			$vars['title']=lng('Installation error');
			$vars['msg']='<div>'.lng('Remote machine has failed to pass the update process. Try pressing F5 or "Refresh".').'</div>';
			$vars['msg'].='<div>'.lng('Ensure that the parent directory has all the necessary privileges to create and delete files and try again.').'</div>';
			$vars['msg'].='<div>'.lng('If the error reappears try to clear ftp server of the remote machine, or use the manual installation').'</div>';
			$vars['type']='error';
			echo shell_tpl_admin('block/message_box',$vars);
		} else {
			$vars['title']=lng('The system was successfully installed');
			$vars['msg']='<div>'.lng('It is recommended to wait a few minutes. At this time the remote system is updating all the files.').'</div>';
			$vars['msg'].='<div><a href="'.$url.'admin/settings">'.lng('Customize username and password for root').'</div>';
			$vars['msg'].='<div><a href="'.$url.'admin/group">'.lng('Control access policy').'</div>';
			$vars['msg'].='<div><a href="'.$url.'admin/update?action=check2">'.lng('Transfer modules and components').'</div>';
			$vars['msg'].='<div><a href="'.$url.'admin/zones">'.lng('Go to the areas management').'</div>';
			$vars['type']='success';
			echo shell_tpl_admin('block/message_box',$vars);
		}
	}
}

if(!empty($_GET["del_dump"]) && $_GET["del_dump"]==1){
	if(file_exists(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.$_GET["date"].'.sql.gz')){
		unlink(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.$_GET["date"].'.sql.gz');
		$vars['title']=lng('file was successfully removed');
		$vars['type']='success';
		echo shell_tpl_admin('block/message_box',$vars);
	}
}

if(!empty($_GET["del_dump"]) && $_GET["del_dump"]==2){
	if(file_exists(DOCUMENT_ROOT.DEPLOY_PATH.'E5-files-'.$_GET["date"].'.tar.gz')){
		unlink(DOCUMENT_ROOT.DEPLOY_PATH.'E5-files-'.$_GET["date"].'.tar.gz');
		$vars['title']=lng('file was successfully removed');
		$vars['type']='success';
		echo shell_tpl_admin('block/message_box',$vars);
	}
}

if(!empty($_GET["del_dump"]) && $_GET["del_dump"]==3){
	if(file_exists(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.$_GET["date"].'!.php')){
		unlink(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.$_GET["date"].'!.php');
		$vars['title']=lng('file was successfully removed');
		$vars['type']='success';
		echo shell_tpl_admin('block/message_box',$vars);
	}
}

if(!empty($_GET["del_dump"]) && $_GET["del_dump"]==4){
	$fls=scan_dir(DOCUMENT_ROOT.'/core/backup',Array(),0,2);
	foreach($fls AS $fl)if(strstr($fl,'.sql.gz')){
		unlink($fl);
	}
	rmdir(DOCUMENT_ROOT.'/core/backup');
}

$fls=scan_dir(DOCUMENT_ROOT.'/files/deploy',Array(),0,2);
$d=''; $d2='';$d3='';
foreach($fls AS $fl){
	if(strstr($fl,'.sql.gz') && strstr($fl,'E5-')){
		$x=str_replace(DOCUMENT_ROOT.DEPLOY_PATH,'',$fl);
		$d.=$x.' (<a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.$x.'">'.lng('download').'</a> '.smart_size(filesize($fl)).', <a href="deploy?del_dump=1&date='.get_tag($x,'E5-','.sql.gz').'">'.lng('remove').'</a>)<br>';
	}
	if(strstr($fl,'.tar.gz') && strstr($fl,'E5-files-')){
		$x=str_replace(DOCUMENT_ROOT.DEPLOY_PATH,'',$fl);
		$d2.=$x.' (<a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.$x.'">'.lng('download').'</a> '.smart_size(filesize($fl)).', <a href="deploy?del_dump=2&date='.get_tag($x,'E5-files-','.tar.gz').'">'.lng('remove').'</a>)<br>';
	}
	if(strstr($fl,'!.php') && strstr($fl,'E5-')){
		$x=str_replace(DOCUMENT_ROOT.DEPLOY_PATH,'',$fl);
		$d3.=$x.' (<a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.$x.'">'.lng('download').'</a> '.smart_size(filesize($fl)).', <a href="deploy?del_dump=3&date='.get_tag($x,'E5-','!.php').'">'.lng('remove').'</a>)<br>';
	}
	if(strstr($fl,'!.tar.gz') && strstr($fl,'E5-')){
		$x=str_replace(DOCUMENT_ROOT.DEPLOY_PATH,'',$fl);
		$d3.=$x.' (<a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.$x.'">'.lng('download').'</a> '.smart_size(filesize($fl)).', <a href="deploy?del_dump=3&date='.get_tag($x,'E5-','!.tar.gz').'">'.lng('remove').'</a>)<br>';
	}
}
if(!empty($d) && empty($_GET["build2"])){
	$vars['title']=lng('Warning!');
	$vars['msg']=lng('Available downloading database dumps for outside users. Remove them after download.').'<br><br>'.$d.'<br>&nbsp;';
	$vars['type']='warning';
	echo shell_tpl_admin('block/message_box',$vars);
}
if(!empty($d2) && empty($_GET["build3"])){
	$vars['title']=lng('Warning!');
	$vars['msg']=lng('Available downloading file dumps for outside users. Remove them after download.').'<br><br>'.$d2.'<br>&nbsp;';
	$vars['type']='warning';
	echo shell_tpl_admin('block/message_box',$vars);
}
if(!empty($d3) && empty($_GET["build"])){
	$vars['title']=lng('Warning!');
	$vars['msg']=lng('Available downloading system builds for outside users.').'<br><br>'.$d3.'<br>&nbsp;';
	$vars['type']='warning';
	echo shell_tpl_admin('block/message_box',$vars);
}

if(file_exists(DOCUMENT_ROOT.'/core/backup')){
	global $zone_url;
	$fls=scan_dir(DOCUMENT_ROOT.'/core/backup',Array(),0,2);
	$d4='';
	$total=0;
	rsort($fls);
	foreach($fls AS $fl)if(strstr($fl,'.sql.gz')){
		$d4.='';
		$x=str_replace(DOCUMENT_ROOT.'/core/backup/','',$fl);
		$y=filesize($fl);
		$total+=$y;
		$d4.=get_normal_date(get_tag($x,'E5-','.sql.gz')).' (<a href="'.$zone_url.'/ajax?action=get_sql_data&amp;x='.$x.'">'.lng('download').'</a> '.smart_size($y).')<br>';
	}
	if(!empty($d4)){
		echo '<p><b>'.lng('There are available database dumps').'</b> - <span onClick="showhide(\'dumps\');" class="link">'.lng('show').'</span><p>';
		echo '<div id="dumps" style="display: none;">'.$d4.'<br>'.lng('Total size of the files').': '.smart_size($total).' (<a href="deploy?del_dump=4">'.lng('remove all').'</a>)</div>';
		echo '<br>';
	}
}

if(!empty($_GET["build4"])){
	check_deploy_path();
	echo '<div id="sst">'.lng('Please do not refresh the window until complete the process. If the server has closed the connection, go to this section through the settings (without refreshing it) and watch the size of packed files (updating this page every 2 minutes) as soon as the file size will stop growing - download it, it means that the process was completed in the background').'</div>';
	ini_set('max_execution_time',0);
	if(file_exists(DOCUMENT_ROOT.'/core/install/package.sql')) copy(DOCUMENT_ROOT.'/core/install/package.sql',DOCUMENT_ROOT.'/core/install/package_tmp.sql');
	backup_db(DOCUMENT_ROOT.'/core/install/package.sql',30000,Array(),Array('auth_session','visit_data','visit_ip','visit_object','visit_source'),1,0);
	
	//getrow($db,"SELECT * FROM main_zone WHERE zone_module=-2",1,"main_zone");
	//$upd='http://'.$_SERVER["HTTP_HOST2"].'/'.$db->Record["zone_folder"];	
	$upd=$_GET['new_userver'];
	if(empty($_GET['use_tar'])){
		$t=gzip_folders(Array('','!core','!files'),Array('config.inc','timing.inc'),Array(),1,1,lng('The system is unpacked').'. <a href="?install=1&amp;host='.$upd.'">'.lng('Continue').'</a>',0,1,1,0,1,'',array_flip(Array('core/safe','core/backup','core/cache','core/log','files/deploy')));
		$f=fopen(DOCUMENT_ROOT.DEPLOY_PATH.'E5-complete-'.date('Y-m-d').'!.php','w');
		fwrite($f,$t);
		fclose($f);
	} else gzip_folders2(DOCUMENT_ROOT.DEPLOY_PATH.'E5-complete-'.date('Y-m-d').'!.tar.gz',Array(/*'core','files','index.php','.htaccess'*/''),1,1,Array('files/deploy','files/compile','core/cache','core/config.inc','core/timing.inc'));
	
	unlink(DOCUMENT_ROOT.'/core/install/package.sql');
	if(file_exists(DOCUMENT_ROOT.'/core/install/package_tmp.sql')){
		copy(DOCUMENT_ROOT.'/core/install/package_tmp.sql',DOCUMENT_ROOT.'/core/install/package.sql');
		unlink(DOCUMENT_ROOT.'/core/install/package_tmp.sql');
	}
	echo '<script>document.getElementById("sst").style.display="none";</script>';
	echo '<div>'.lng('The file is saved at the address').': <a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-complete-'.date('Y-m-d').'!.'.(empty($_GET['use_tar'])?'php':'tar.gz').'">http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-'.date('Y-m-d').'!.'.(empty($_GET['use_tar'])?'php':'tar.gz').'</a>. '.lng('Do not forget to rename the file to index.php before uploading it to the server. The file name must not contain an exclamation point (otherwise the file will be offering yourself to download)').'</div>';
}

if(!empty($_GET["build3"])){
	check_deploy_path();
	echo '<div id="sst">'.lng('Please do not refresh the window until complete the process. If the server has closed the connection, go to this section through the settings (without refreshing it) and watch the size of packed files (updating this page every 2 minutes) as soon as the file size will stop growing - download it, it means that the process was completed in the background').'</div>';
	ini_set('max_execution_time',0);
	gzip_folders2(DOCUMENT_ROOT.DEPLOY_PATH.'E5-files-'.date('Y-m-d').'.tar.gz',Array('files'),1,0,Array('files/deploy','files/compile'));
	echo '<script>document.getElementById("sst").style.display="none";</script>';
	echo '<div>'.lng('The file is saved at the address').': <a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-files-'.date('Y-m-d').'.tar.gz">http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-files-'.date('Y-m-d').'.tar.gz</a>. '.lng('Do not forget to delete the file after download').' (<a href="deploy?del_dump=2&date='.date('Y-m-d').'">'.lng('remove').'</a>)</div>';
}

if(!empty($_GET["build2"])){
	check_deploy_path();
	backup_db(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.date('Y-m-d').'.sql.gz');
	echo '<div>'.lng('The file is saved at the address').': <a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-'.date('Y-m-d').'.sql.gz">http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-'.date('Y-m-d').'.sql.gz</a>. '.lng('Do not forget to delete the file after download').' (<a href="deploy?del_dump=1&date='.date('Y-m-d').'">'.lng('remove').'</a>)</div>';
}

getrow($db,"SELECT * FROM main_zone WHERE zone_module=-2",1,"main_zone");
$userver='http://'.$_SERVER["HTTP_HOST2"].'/'.$db->Record["zone_folder"];

if(!empty($_GET["build"])){
	check_deploy_path();
	$upd=$userver;
	$t=gzip_folders($GLOBALS["UPDATE_PATHS"],Array('config.inc','timing.inc'),Array('files/pub','files/uploads','files/design'),1,1,lng('The system is unpacked').'. <a href="?install=1&amp;host='.$upd.'">'.lng('Continue').'</a>',0,1,1,0,1);
	$f=fopen(DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.date('Y-m-d').'!.php','w');
	fwrite($f,$t);
	fclose($f);
	echo '<div>'.lng('The file is saved at the address').': <a href="http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-'.date('Y-m-d').'!.php">http://'.$_SERVER["HTTP_HOST2"].DEPLOY_PATH.'E5-'.date('Y-m-d').'!.php</a>. '.lng('Do not forget to rename the file to index.php before uploading it to the server. The file name must not contain an exclamation point (otherwise the file will be offering yourself to download)').'</div>';
}

if(!$ok && (empty($action) || $action=='install')){

	if(empty($ftp_folder)) $ftp_folder='/';
	if(empty($ftp_server)) $ftp_server='';
	if(empty($ftp_login)) $ftp_login='';
	if(empty($ftp_password)) $ftp_password='';
	if(empty($mysql_server)) $mysql_server='localhost';
	if(empty($mysql_login)) $mysql_login='';
	if(empty($mysql_password)) $mysql_password='';
	if(empty($mysql_database)) $mysql_database='';
	if(empty($url)) $url='http://www.site.ru/';
	$add='';if(!empty($mysql_create)) $add=' checked';
	$norm=true;
	//getrow($db,"SELECT * FROM main_zone WHERE zone_module=-2 AND zone_active=1",1,"main_zone");
	getrow($db,"SELECT * FROM main_zone WHERE zone_module=-2",1,"main_zone");
	if(empty($db->Record)){
		$norm=false;
		echo '<br><div><b>'.lng('To install the system, you must have area with update server').'</b></div>';
	}
	if($norm){
		$vars['form_type']='add';
		$vars['btn_title']=lng('Install');
		$vars['name']=lng('System installation to another server by FTP');
		$vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'action', 'value'=>'install');
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'url', 'title'=>lng('URL where system will be called <br> (setting necessary only for verification and further will not be used)'), 'value'=>$url);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'ftp_server', 'title'=>lng('FTP server'), 'value'=>$ftp_server);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'ftp_login', 'title'=>lng('FTP login'), 'value'=>$ftp_login);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'ftp_password', 'title'=>lng('FTP password'), 'value'=>$ftp_password);
		$vars['section']['main']['fields'][]=Array('type'=>'hidden', 'name'=>'ftp_folder', 'value'=>$ftp_folder);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'mysql_server', 'title'=>lng('MySQL server'), 'value'=>$mysql_server);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'mysql_login', 'title'=>lng('MySQL login'), 'value'=>$mysql_login);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'mysql_password', 'title'=>lng('MySQL password'), 'value'=>$mysql_password);
		$vars['section']['main']['fields'][]=Array('type'=>'text', 'name'=>'mysql_database', 'title'=>lng('MySQL database'), 'value'=>$mysql_database);
		echo shell_tpl_admin('block/form',$vars);
		
		echo '<br><br><a href="deploy?build=1">'.lng('Compile build of the current system').'</a>';
		echo '<br><br><a href="deploy?build2=1">'.lng('Compile database dump').'</a>';
		echo '<br><br><a href="deploy?build3=1">'.lng('Pack public files').'</a><br>('.lng('the process may not be completed on typical shared hosting, if the server has issued 504 error, go to this section after 5-10 minutes and possibly the file is ready (do not update build process)').')';
		
		echo '<br><br><div style="background-color: #EEEEEE; padding: 10px;"><form action="deploy"><p><b>'.lng('Clone system (Pack db & files ready to install)').'</b></p><p>'.lng('Update server').': <input type="text" id="userver" name="new_userver" style="width: 300px;" value="'.$userver.'"></p><p><label style="cursor: pointer;"><input type="checkbox" id="use_tar" name="use_tar" class="checkbox">'.lng('Compress as TAR (Update server will be ignored)').'</label></p><p><input type="hidden" name="build4" value="1"><input type="submit" value="'.lng('Pack').'"></p></div>';
		
	}

}

?>