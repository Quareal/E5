<?php

	function sdefine($name,$value){
		if(!defined($name)) define($name,$value);
	}
	function gdefine($name,$value){
		$GLOBALS[$name]=$value;
	}

	// Определение директорий
	sdefine("DOCUMENT_ROOT", dirname(dirname(dirname(__FILE__))));	// в качестве альтернативы можно использовать SERVER["SCRIPT_FILENAME"]
	sdefine('FTEMP',DOCUMENT_ROOT.'/core/cache/');					// путь к папке с кешем и прочей временной информации
	sdefine('JTEMP',DOCUMENT_ROOT.'/core/cache/json/');				// путь к папке с кешем JSON объектов для визуального редактора
	sdefine('CTEMP','/files/compile/');									// путь к папке с содержимым секций COMPILE
	gdefine('CACHE_FOLDERS',Array(DOCUMENT_ROOT.CTEMP,FTEMP)); // пути всех временных папок
	
	// Три зарезервированных символа
	sdefine('SEP1','‡');
	sdefine('SEP2','‹');
	sdefine('SEP3','†');
	
	// Символы вертикальных пробелов
	sdefine("VSP", chr(10));
	sdefine("VSP2", '
');

	
	// Пути, участвующие в обновлении и развёртыванию системы
	
	//gdefine('UPDATE_PATHS',Array('','core','core/units','core/editor','core/install','core/update','core/editor/tpl','core/editor/mod_main', 'core/units/template', 'core/units/template/cmd', 'core/units/left','files/editor','files/.htaccess','files/editor/classic','files/editor/icons','files/editor/master','files/editor/ace','!files/js/elfinder','!core/editor/components','core/units/regional'));
	gdefine('UPDATE_PATHS',Array('','!core', '!files/editor','!files/js/elfinder'));//! prefix mean include sunfolders
	gdefine('UPDATE_FILES',Array('.htaccess','core/.htaccess','files/.htaccess'));
	// обратите внимание, что files/.htaccess дублируется и в файлах и в папках, первое необходимо для создания сборки системы, второе - для полноценного обновления
	sdefine('DEPLOY_PATH','/files/deploy/');
	gdefine('DISALLOW_PATHS',Array('core/safe','core/backup','files/editor/preview','files/editor/modules','core/cache','core/log','files/deploy','files/compile'));
	$GLOBALS["DISALLOW_PATHS"]=array_flip($GLOBALS["DISALLOW_PATHS"]);
	gdefine('DISALLOW_FILES',Array('core/config.inc','core/timing.inc'));
	$GLOBALS["DISALLOW_FILES"]=array_flip($GLOBALS["DISALLOW_FILES"]);
	gdefine('LNG_PATHS',Array('core/editor/lng','core/units/lng'));
	
	// Файлы
	
	sdefine('DEPLOY_HTACCESS',
'<FilesMatch ".(php)$">
 Order Allow,Deny
 Allow from all
</FilesMatch>'
	);
	
	sdefine('BASE_HTACCESS',
'ErrorDocument 404 /index.php
ErrorDocument 403 /index.php
ErrorDocument 402 /index.php

RewriteEngine  on
RewriteCond %{REQUEST_FILENAME}   !-d
RewriteCond %{REQUEST_FILENAME}   !-f
RewriteRule ^(.*)$ index.php [L]
Options -Indexes');	

	sdefine('FILES_HTACCESS',
'ErrorDocument 404 default
ErrorDocument 403 default
ErrorDocument 402 default

RewriteEngine  off
Options -Indexes

<FilesMatch ".(php|php6|php5|php4|php3|phps|phtml|phpt)$">
 Order Allow,Deny
 Deny from all
</FilesMatch>');

	// Валидные юзер-агенты
	$br='mozilla,amaya,msie,blackberry,links,lynx,mmp,opera,w3m,chrome,safari,webkit,gecko,presto,maxthon,netscape';
	$br=explode(',',$br);
	gdefine('user_agents',$br);

?>