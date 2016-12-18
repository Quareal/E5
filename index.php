<?php

	// Добро пожаловать в исходный код CMS E5

	// любые замечания, пожелания, вопросы по сотрудничеству или локализации высылайте на электронный адрес support@rucms.org
	// подробности на сайте RuCMS.org
	
	// система распространяется бесплатно
	
	// =======================
	
	// Содержимое корневой директории
	// - index.php - главный файл системы
	// - files - папка для файлов
	// - core - файлы ядра системы
	// - .htaccess - настройки для RewriteEngine
	
	// =======================
	
	// Если система не запускается
	// - удалите секцию "Настройки PHP" (ниже, все функции ini_set)
	// - поставьте знак # перед Options -Indexes в файле .htaccess
	// - проверьте права файлов .php (установите их в 644)
	// - убедитесь, что ваш сервер обладает минимальными требованиями (ModRewrite, PHP не ниже 4 версии, 32мб ОЗУ)
	
	// =======================	
	
	// Проверка наличия жёсткого кеша

	$uri_md5=md5($_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI']);
	if(file_exists('core/cache/h/'.$uri_md5)){		
		if(file_exists('core/cache/h/'.$uri_md5.'_static')){
			foreach(unserialize(file_get_contents('core/cache/h/'.$uri_md5.'_static')) AS $value) header($value);
		}
		readfile('core/cache/h/'.$uri_md5);
		exit;
	}
	
	// =======================	

	// Этап 1. Настройки системы по умолчанию
	
	// бОльшая часть настроек перекрывается значениями из файла /core/config.inc
	// рекомендуется менять настройки из соответствующего раздела административного кабинета
	// для этого необходимо обладать полномочиями суперпользователя
	
	// Настройки debug-режима
	
		$debug=false;		// debug-режим
	
		$debug2=false;		// расширенный debug-режим
	
		$iqs=false;		// включить в подсчёт выполнения функций время, затраченное на исполнение SQL-запросов
	
		$ip_debug='';		// IP для debug-режима. Если не установлен - debug-режим будет работать с любого IP
		
		$do_proc_log=false;	// предоставление расширенной информации в терминале (привязка IP адресов клиентов к PID процессов сервера)
		
		$do_sql_log=false;	// включение логирование SQL запросов в файл /core/log/sql.log
		
		$do_part_log=false;	// включение записи количества и времени запуска всех частей
	
	// Настройки кеширования
	
		$cache_sql=false;	// кеширование результатов простой выборки SQL в во всех таблицах, за исключением массива DNCT (см. ниже)
		$dnct=array_flip(Array('auth_perm','auth_link','main_auth','main_mail','main_news','main_row','row_owner','row_user','row_value','row_index','index_word','visit_ip','visit_object','visit_data','visit_source'));
		$use_dnct=true;	// если установить значение в FALSE, то перечень DNCT не будет учитываться при кешировании (включите эту опцию, если на вашем ресурсе хранится не обновляемый контент)
	
		$cache_tpl=false;	// абсолютное кеширование результатов выполнения всех шаблонов
	
		$use_globr=false;	// использовать преждевременную догрузку данных
						// снижает количество запросов к БД, рекомендуется использовать на проектах с малым количеством контента
					
		$cache_tpl2=true;	// подготовка всех шаблонов к исполняемому виду
						// ускоряет дальнейшие вызовы шаблонов, превращает шаблон из текстового вида в объект, которых хранится в сериализованном виде на жёстком диске сервера
					
		$no_cache=false;	// включение режима экономии оперативной памяти
	
		$cache_sql2=0;	// режим исключения повторных запросов к БД (внимание! не поддерживается на sqli, требует дополнительного объёма оперативной памяти)
		
		$hard_cache=false;	// полное кеширование страницы (включается командой [hard_cache] в нужном обработчике)
			
		// после ручного отключения кеша не забудьте очистить папку CORE/CACHE
	
	// Настройки CRON
	
		$cron_type=1;		// тип вызова отложенных скриптов CRON
						// 0 - вызывается внешней CRON службой сервера (стандартный вариант)
						// 1 - вызывается посетителями не чаще 1 раза в 5 минут
						// (благодаря включению ignore_user_abort, данная операция остаётся незаметна для посетителей)

						// Адрес для исполнения CRON-скриптов можно задать двумя способами
						// 1. Прямое обращение к /core/cron.php  (например "~/public_html/core/cron.php")
						// 2. Обращение через WGET (или подобную систему) к http://your-site.ru?cron=1 , где your-site.ru - адрес вашего домена, к которому прикреплена система (например  "wget -O - -q http://site1.ru?cron=1")
					
		$cron_su=0;		// права для скриптов CRON
						// 0 - выполнять скрипты CRON от имени гостя
						// 1 - выполнять скрипты CRON от имени суперпользователя
					
		$cron_jobs=1;		// количество задач, которые выполняются во время вызова CRON скрипта
	
	
	
		$correct_utf=1;		// корректировка UTF (заставляет сервер БД работать в кодировке utf8_general_ci)
					
	// Настройки доступа

		$def_root='pub';	// путь по умолчанию для файлового редактора (начная от "files")
						// оставьте пустым для доступа к "files" по умолчанию
		
		$def_chmod=0644;	// права доступа для файлов по умолчанию
						// если вы планируете доступ к файлам через сторонниго пользователя (например, через FTP-клиент), установите права 666 здесь или в меню "Настройка"
		
		$def_drmod=0755;	// права доступа для папок по умолчанию
						// если вы планируете доступ к файлам через сторонниго пользователя (например, через FTP-клиент), установите права 777 здесь или в меню "Настройка"
		
		$su_safe='';		// значение по умолчанию для переменной-протекции защищённой сессии суперпользователя	
		
		$check_for_xss=1;	// защищать формы от отправки JavaScript XSS (для общих зон и аякса)
		
		$black_ips=array_flip(Array()); // массив блокируемых IP адресов	
		
	// Настройки статистики посещаемости

		$statistics=1;		// вести статистику (может нагружать базу данных)
		
		$del_ip_history=1;	// удалять историю статистики по IP адресам по прошествии 24 часов
		
		$del_hour_history=0; // удалять историю по часам по прошествии 48 часов
		
		$del_day_history=0; // удалять историю по дням по прошествии 62 дней
		
		$del_source_history=0; // удалять историю по источникам по прошествии 62 дней
		
		$collect_object_history=1; // собирать историю посещений объектов
		
		$collect_sources_history=1; // собирать сведения об источниках посетителей (домен + URL длинной <= 200симв)
		
		$ignore_bot=1;	// игнорировать пользователей для статистики, у которых user_agent не соответствует ни одной ключевой фразе из $user_agents (определяется в constants.inc)
		
		$stop_stat=false;	// не учитывает статистику (значение может перекрываться функцией [stat.stop])

	
	// Прочие настройки
	
		$local_region='ru';	// регион для подключения особых региональных процедур
		
		$local_lng='ru';		// язык работы системы и административного кабинета (в дальнейшем это значение может перекрываться отдельно для каждого пользователя административного кабинета)
	
		$is_offline=0;		// включение режима offline для тестирования системы в локальной сети.
						// проверка локации пользователя в режиме offline не работает
											
		$no_update=array_flip(explode('|',''));	//файлы, запрещённые для обновления (пути указывать через знак |)
	
		$do_backup=0;		// включение автоматического архиватора базы данных (запускается 1 раз в сутки)
		$do_backup_files=0;	// включение автоматического архиватора файлов пользователя (папка /files)
	
		$def_charset='utf-8';	// принудительная отправка кодировки сервером
		
		$index_at_once=20;	// максимальное количество текстов, индексируемые за одну работу скрипта
						// индексация происходит для текстовых полей, у которых включено индексирование
						// индексация происходит в момент открытия любой страницы административного кабинета, либо по CRON-у
		$max_new_lemma_at_once=500; //максимальное кол-во лемм при индексации (если значение переполняется - индексация прерывается на текущем индексируемом объекте и продолжается только с следующим стартом индексатора)
		
		$protect_admin_form=1;	// защищать формы административного раздела от повторной отправки данных при обновлении страницы		
		
		$database_type='';	// тип инструкций, для доступа к базе данных: mysql, mysqli
						// если не указано - выбирается автоматически
		
		$send_error_reports='';	// email адрес, на который уходят сообщения об ошибках (не чаще раза в 5 минут)
		
		$show_404=true;		// отправлять 404 заголовок в случае, если имеется неразобранный аппендикс URL
		
		$session_lifetime=0;		// максимальное время ожидания сессии в минутах (0 - бесконечность, перекрывается в настройках пользователя)
		$session_multy=1;		// разрешать больше одной сессии для пользователя
		$session_storage=5;		// количество хранящихся архивных сессий для пользователя (0 - не хранить, -1 - не удалять отработанные сессии)
		$session_everytime=false;	// генерировать новый ключ сессии при каждом новом соединении
		$protect_form_by_session=true;	// использовать ли ключ ID сессии для защиты форм административного кабинета, если FALSE, то принудительно убирает ID сессии из ключа безопасности
		
		$auth_single_max_try=10;	// максимальное число попыток входа для одного логина до наступления периода охлаждения auth_cooling, 0 - бесконечно
		$auth_all_max_try=100;		// максимальное число попыток входа для разных логинов до наступления периода охлаждения auth_cooling, 0 - бесконечно
		$auth_cooling=5;			// кол-во минут, в течении которых пользователь не сможет авторизоваться в системе в том случае, если перебрано больше auth_max_try паролей
		
		$use_overload_signal=false;	// посылать на электронную почту администратора сообщения о перегрузке сервера
		$overload_value=10;		// значение перегрузки, после которого будет отправлено сообщение		
		$halt_on_overload=false;		// отдавать ошибку 503 в случае перегрузки сервера
		
		$superuser_ip='';			//IP адрес суперпользователя
		
		$max_cache_size_mb=0;		//Максимальный размер кеша в мегабайт (проверка 1 раз в час, 0 - нет ограничений)
	
	// Параметры PHP и прочие параметры среды
		$is_console = PHP_SAPI == 'cli' || (!isset($_SERVER['DOCUMENT_ROOT']) && !isset($_SERVER['REQUEST_URI']));
		// поддержка FCGI режима с отключённым GET
		/*if(isset($_SERVER['FCGI_ROLE']) && isset($_SERVER["REQUEST_URI"])){
			$tmp=explode('?',$_SERVER["REQUEST_URI"]);
			if(!empty($tmp[1])){
				$tmp=explode('&',$tmp[1]);
				if(!empty($tmp)) foreach($tmp AS $t){
					$t=explode('=',$t);
					if(!isset($t[1])) $t[1]='';
					$_GET[$t[0]]=$t[1];
				}
			}
		}*/
		
		if((isset($_SERVER['HTTP_X_FORWARDED_PROTO']) && $_SERVER['HTTP_X_FORWARDED_PROTO']=='https') || !empty($_SERVER['HTTPS'])) $prot='https'; else $prot='http';
	
	// Настройки, утратившие свою актуальность
		$use_ace=0;		// использовать по умолчанию Ace Editor для правки частей модуля		
		$cache_func=false;	// кеширование результатов текстовых функций
		
	// (если на вашем сервере запрещено перекрытие конфигурации веб-сервера, удалите данную секцию)
	// ниже, некоторые из этих значений могут перекрываться значениями из конфигурационного файла
	//	ini_set('upload_max_filesize','50M');					// максимальный размер файла для загрузки
	//	ini_set('post_max_size','100M');					// суммарный максимальный размер всех загружаемых файлов
	//	if(!$is_console) ini_set('max_execution_time','100');		// максимальное время работы PHP скрипта
	//	ini_set('memory_limit','300M');						// лимит памяти ОЗУ
	//	ini_set('php_admin_value mbstring.func_overload','0');	// корректная работа с UTF строками
		
	// Этап 2. Начало работы

	if(file_exists('core/config.inc') || (!file_exists('core/config.inc') && (!empty($_GET['install']) || !empty($_POST['install'])))) error_reporting(0);	// при включёном debug-режиме данное значение перекрывается
	
	// отдаём заголовок HTTP
	if(empty($_GET["cron2"]) && isset($_SERVER["REDIRECT_STATUS"]) && $_SERVER["REDIRECT_STATUS"]=='404') header ("HTTP/1.1");
	
	// стартуем таймер начала работы	
	$mtime=explode(" ", microtime());$start_time=$mtime[1]+$mtime[0];
	
	// определяем константы
	include('core/units/constants.inc');
	$user=''; // пользователь по умолчанию - гость
	
	// игнорируем запрос FavIcon из корня
	if(!empty($_SERVER['REQUEST_URI']) && $_SERVER['REQUEST_URI']=='/favicon.ico' && !file_exists(DOCUMENT_ROOT.'/favicon.ico')) exit;

	// загружаем файлы системы
	include('core/units/orm.inc');			// функции для работы с объектами системы
	include('core/units/strings.inc');			// строковые функции
	include('core/units/search.inc');			// функции для работы поиска
	include('core/units/date.inc');			// функции даты времени
	include('core/units/template/main.inc');	// шаблонизатор		
	include('core/units/files.inc');			// файловые функции
	include('core/units/images.inc');			// функции работы с изображением
	include('core/units/internet.inc');			// фукнции для работы с интернетом
	include('core/units/statistics.inc');		// фукнции для сбора статистики
	include('core/units/orm_forms.inc');		// дополнительный набор функций для работы с объектами системы
	include('core/units/url.inc');			// функции для работы с URL
	include('core/units/users.inc');			// функции для работы с пользователями
	
	global $restore;
	$restore=''; 
	
	// проверка режима работы (из корневой папки домена / из подпапки)
	if(!empty($_SERVER["SCRIPT_FILENAME"])) $file_url=$_SERVER["SCRIPT_FILENAME"];
	else $file_url=__FILE__;
	$base_root=seek_baseroot($file_url);
	
	// если не найден файл config.inc - запускаем процесс установки
	// желательна ещё одна проверка, на случай неудачной установки
	if(!file_exists('core/config.inc')) include('core/install/install.php');
	if(!is_readable('core/config.inc')) chmod('core/config.inc',0777); // если не удаётся считать файл настроек, пытаемся установить на него права 777
	
	// включаем настройки из файла config.inc
	include('core/config.inc');
	if(file_exists('core/timing.inc') || isset($skip_zone_check)) include('core/timing.inc');
	
	// проверка на перегрузку	
	if($use_overload_signal && get_loadavg()!='' && $GLOBALS['load_average']>=$overload_value){
		fire('server-overload',$GLOBALS['load_average'],false);
		if($halt_on_overload){
			header("HTTP/1.1 503 Service Unavailable");
			exit;
		}
	}
	
	// перекрываем значения некоторых настроек	
	if(!empty($rewrite_upload_max_filesize)) ini_set('upload_max_filesize',$rewrite_upload_max_filesize.'M');
	if(!empty($rewrite_post_max_size)) ini_set('post_max_size',$post_max_size.'M');
	if(!empty($rewrite_memory_limit)) ini_set('memory_limit',$rewrite_memory_limit.'M');
	if(!empty($rewrite_max_execution_time)){
		if($rewrite_max_execution_time==-1) ini_set('max_execution_time',0);
		else ini_set('max_execution_time',$rewrite_max_execution_time);
	}
	if(!empty($rewrite_max_file_uploads)){
		if($rewrite_max_file_uploads==-1) ini_set('max_file_uploads',0);
		else ini_set('max_file_uploads',$rewrite_max_file_uploads);
	}		
	
	// проверка наличия IP текущего пользователя в чёрном списке
	// если наличие подтверждено - пользователь получит 503 ошибку
	if(isset($_SERVER["REMOTE_ADDR"]) && isset($black_ips[$_SERVER["REMOTE_ADDR"]])){
		header("HTTP/1.1 503 Service Unavailable");
		exit;
	}
	
	// устанавливаем права для новых/обновлённых папок и файлов по умолчанию
	if(!defined('DEF_CHMOD')) define('DEF_CHMOD',$def_chmod);
	if(!defined('DEF_DRMOD')) define('DEF_DRMOD',$def_drmod);
	
	// включение отчёта об ошибках в режиме debug
	if($debug && ((!empty($_SERVER["REMOTE_ADDR"]) && $_SERVER["REMOTE_ADDR"]==$ip_debug) || !$ip_debug)){
		//define('REPORTING_LEVEL',E_ALL & ~E_STRICT);
		define('REPORTING_LEVEL',E_ERROR | /*E_WARNING |*/ E_PARSE | E_NOTICE);
		ini_set('display_errors',1);
	} else {
		define('REPORTING_LEVEL',0);
		ini_set('display_errors',0);
	}
	error_reporting(REPORTING_LEVEL);
	
	// подключение функций для работы с БД
	include('core/units/db.inc');
	
	// проверка жёсткого кеширования с статическим компонентом	
	/*if(file_exists('core/cache/h/'.$uri_md5)){
		echo shell_tpl(file_get_contents('core/cache/h/'.$uri_md5.'_static'));
		readfile('core/cache/h/'.$uri_md5);
		exit;
	}*/
	
	// включение логирования PID и IP адресов на *nix системах
	if($do_proc_log) proc_log();	
		
	// восстановление структуры данных в БД
	if(!empty($restore)){
		if(!empty($_POST["remove"]))	remove_db($GLOBALS["database"]);
		if(!table_exists('auth_link')){
			if(empty($GLOBALS['dump_from_file'])) load_dump($dump,$GLOBALS["db"]);
			else load_dump('',$GLOBALS["db"],0,$dump);
		}
	}
	
	// установка базового почтового адреса
	if(empty($system_email) && !empty($_SERVER["HTTP_HOST"])){$v=explode('.',$_SERVER["HTTP_HOST"]); $system_email='system@'.$v[count($v)-2].'.'.$v[count($v)-1]; save_config();}
	
	// установка базовой соли (solt) системы (для генерации хешей от паролей, запускается если соль ещё не установлена)
	if(empty($e5uid)) save_config();
	
	// определяем - не наступил ли новый день и час?
	$do_save_cfg=false;
	if(empty($last_start_date) || $last_start_date!=date('Y-m-d')){
		$GLOBALS["is_new_day"]=true;
		$do_save_cfg=true;
		//save_config();
	} else $GLOBALS["is_new_day"]=false;
	if(empty($last_start_hour) || $last_start_hour!=date('Y-m-d H')){
		$GLOBALS["is_new_hour"]=true;
		$do_save_cfg=true;		
	} else $GLOBALS["is_new_hour"]=false;	
	if($do_save_cfg) save_timing();//save_config();
	
	// расшифровка кодированного URL
	if(empty($_SERVER["REQUEST_URI"])) $_SERVER["REQUEST_URI"]='';
	else {
		$_SERVER["REQUEST_URI"]=deshurl($_SERVER["REQUEST_URI"]);
	}
	
	// отключаем magic_quotes_runtime
	if(get_magic_quotes_runtime()) set_magic_quotes_runtime(false);
	
	// принудительное включение защиты от XSS
	if(isset($_POST["check_xss"]) || isset($_GET["check_xss"])) $check_for_xss=1;
	
	// проверка на повторную отправку формы
	if(isset($_POST["form_protect"]) || isset($_GET["form_protect"])){
		if(isset($_POST["form_protect"])){ $fp=$_POST["form_protect"]; $g=1; }
		else if(isset($_GET["form_protect"])){ $fp=$_GET["form_protect"]; $g=2; }
		if(!isset($_COOKIE["fp".$fp])){
			SetCookie("fp".$fp,"1",time()+3600,'/','',false);	// генерируем куку, по которой будем идентифицировать текущую отправленную форму
		} else {
			// если кука с таким значением уже есть - уничтожаем все POST / GET переменные
			if(empty($_POST["form_protect_type"])){
				if($g==1) $_POST=Array();
				if($g==2) $_GET=Array();
			} else {
				$tkes=Array('id','id2','id3','id4','id5','id6','id7','f','sort','page');
				foreach($_POST AS $var=>$value) if(!in_array($var,$tkes)){
					unset($_POST[$var]);
					if(isset($GLOBALS[$var])) unset($GLOBALS[$var]);
				}
			}
		}
	}
	
	// Если установлена опция magic_quotes_gpc - очищаем её последствия
	clean_magic_quotes();

	$GLOBALS['init_timer']=end_timer($start_time);
	
	
	// Этап 3. Определение текущей зоны / сайта
	
	stf('zone_detect');
	// выгружаем список зон из БД
	$zone=getall($db,"SELECT * FROM main_zone ORDER BY zone_name",1,'main_zone');
	$zone2=Array(); for($i=0;$i<count($zone);$i++) $zone2[$zone[$i]["zone_id"]]=$zone[$i];
	
	if(empty($_SERVER["SERVER_NAME"])) $_SERVER["SERVER_NAME"]='';
	if(empty($_SERVER["HTTP_HOST"])) $_SERVER["HTTP_HOST"]="";
	
	// Обработка REQUEST_URI с учётом подпапки из которой работает система
	if(!empty($base_root) && substr($_SERVER["REQUEST_URI"],0,strlen($base_root))==$base_root){
		$_SERVER["REQUEST_URI"]=substr($_SERVER["REQUEST_URI"],strlen($base_root));		
	}
	
	// Дальнейшая очистка и обработка REQUEST_URI
	$ruri=$_SERVER["REQUEST_URI"];
	if(strpos($ruri,'?')){
		$ruri=explode('?',$ruri);
		$ruri=$ruri[0];
	}
	if($ruri=='/')$ruri='//';$ruri2=$ruri;if(strlen($ruri)==0 || $ruri[strlen($ruri)-1]!='/') $ruri.='/';if(!empty($ruri2) && $ruri2[strlen($ruri2)-1]=='/') $ruri2=substr($ruri2,0,strlen($ruri2)-1);
	$ruri=str_replace("'",'`',$ruri);
	$ruri=urldecode($ruri);
	$ruar=explode('/',$ruri);
	$lruar=$ruar[count($ruar)-2];
	if(!isset($_SERVER["SERVER_NAME"]) || $_SERVER["SERVER_NAME"]=='') $_SERVER["SERVER_NAME"]=$_SERVER["HTTP_HOST"];
	
	// Декодирование национальных доменных имён
	if(strstr($_SERVER["SERVER_NAME"],'xn--')){
		include_once('core/units/left/idna_convert.class.php');
		$IDN = new idna_convert();
		$encoded = $IDN->decode($_SERVER["SERVER_NAME"]);
		$_SERVER["SERVER_NAME"]=$encoded;
	}
	$GLOBALS["host5"]=$_SERVER["HTTP_HOST"];
	if(strstr($_SERVER["HTTP_HOST"],'xn--')){
		include_once('core/units/left/idna_convert.class.php');
		$IDN2 = new idna_convert();
		$encoded = $IDN2->decode($_SERVER["HTTP_HOST"]);
		$_SERVER["HTTP_HOST"]=$encoded;
	}
	if(!empty($_SERVER["HTTP_REFERER"]) && strstr($_SERVER["HTTP_REFERER"],'xn--')){
		include_once('core/units/left/idna_convert.class.php');
		$IDN2 = new idna_convert();
		$encoded = $IDN2->decode($_SERVER["HTTP_REFERER"]);
		$_SERVER["HTTP_REFERER"]=$encoded;
	}
	
	// Определение браузера пользователя
	if(!empty($_SERVER["HTTP_USER_AGENT"])) $user->browser=GetUserAgent(); else $user->browser='else';
	
	// Запись IP адреса пользователя
	if(!empty($_SERVER["REMOTE_ADDR"])){
		if(isset($_SERVER["SERVER_ADDR"]) && $_SERVER["REMOTE_ADDR"]==$_SERVER["SERVER_ADDR"] && isset($_SERVER["HTTP_X_REAL_IP"])) $user->ip=$_SERVER["HTTP_X_REAL_IP"];
		else $user->ip=$_SERVER["REMOTE_ADDR"];
		$user->ip=safe_sql_input($user->ip);
	} else $user->ip='';
	
	// Перебор зон на предмет совпадения с текущим URL
	$bool=true;
	$seek_multydomain=false;
	$add_to_url=Array();
	$restrict=Array();
	
	while($bool){
		$za=-3;
		// а) совпадение по папке, без учёта домена
		if($za==-3)	for($i=0;$i<count($zone);$i++) if(empty($restrict[$i]) && (!$seek_multydomain || $zone[$i]["zone_autosub"])) if(bstrstr2('/'.$zone[$i]["zone_folder"].'/',$ruri) && $zone[$i]["zone_domain"]=='' && $zone[$i]["zone_active"]==1){$za=$i;break;}
		// б) совпадение по папке и домену
		if($za==-3)	for($i=0;$i<count($zone);$i++) if(empty($restrict[$i]) && (!$seek_multydomain || $zone[$i]["zone_autosub"])) if(bstrstr2('/'.$zone[$i]["zone_folder"].'/',$ruri) && $zone[$i]["zone_domain"]==$_SERVER["SERVER_NAME"] && $zone[$i]["zone_active"]==1){$za=$i;break;}
		// в) совпадение по папке и домену, либо только по папке (если домен у зоны не определён)
		if($za==-3)	for($i=0;$i<count($zone);$i++) if(empty($restrict[$i]) && (!$seek_multydomain || $zone[$i]["zone_autosub"])) if(bstrstr2('/'.$zone[$i]["zone_folder"].'/',$ruri) && ($zone[$i]["zone_domain"]=='' || $zone[$i]["zone_domain"]==$_SERVER["SERVER_NAME"]) && $zone[$i]["zone_active"]==1){$za=$i;break;}
		// г) совпадение только по домену
		if($za==-3)	for($i=0;$i<count($zone);$i++) if(empty($restrict[$i]) && (!$seek_multydomain || $zone[$i]["zone_autosub"])) if($zone[$i]["zone_domain"]==$_SERVER["SERVER_NAME"] && $zone[$i]["zone_folder"]=='' && $zone[$i]["zone_active"]==1){$za=$i;break;}
		// д) зоны, у которых не указан домен и папка
		if($za==-3)	for($i=0;$i<count($zone);$i++) if(empty($restrict[$i]) && (!$seek_multydomain || $zone[$i]["zone_autosub"])) if($zone[$i]["zone_domain"]=='' && $zone[$i]["zone_active"]==1 && $zone[$i]["zone_folder"]==''){$za=$i;break;}
		// поиск автоматических поддоменов
		if($za==-3){
			$sne=explode('.',$_SERVER["SERVER_NAME"]);
			if(count($sne)>2){
				$add_to_url[]=$sne[0];
				array_shift($sne);
				$_SERVER["SERVER_NAME"]=implode('.',$sne);
				$seek_multydomain=true;
				$bool=true; continue;
			}
		}
		// если зона не найдена - запустить CRON (если cron_type==1 и время для CRON подходящее) и завершить работу
		if($za==-3 && !isset($skip_zone_check)){
			if(!empty($_GET["cron"])) include('core/cron.php');
			if($statistics) collect_stat(false);
			etf('zone_detect');
			exit;
		}
		$bool=false;
		
		// проверка зоны в соответствии с выставленными настройками IP
		
			// В проверке участвует IP-адрес, группа IP-адресов, страна или город из которой пользователи будут или не будут иметь доступ
			// Несколько условий складываются по принципу AND и OR, а именно по схеме "X,Y+Z", где X,Y,Z - условия, ","=AND а "+"=OR. Т.е. доступ будет дан, если пользователь подподает под условия X и Y либо под условие Z.
			// Условием может быть IP адрес, знак * (все пользователи), диапозон адресов (например "192.168.1.1-192.169.50.12"), город или страна (на английском, должна соответствовать базе WHOIS), название браузера, заключённое в двойные кавычки (user agent или его часть, без учёта регистра) или часть адреса, с которого был осуществлён переход, заключённый в одиночные кавычки (также без учёта регистра, адрес предварительно проходит процедуру URL-декодирования)
			// Если перед условием стоит знак !, то оно будет иметь обратное действие (например "!rostov" будет блокировать всех посетителей из ростова)
			// Примеры:
			// russia,!rostov+62.189.203.0-70.189.205.0 - доступ будет дан для всех жителей России, кроме жителей Ростова + тех, кто попадёт в указанный диапазон адресов
			// *,!56.43.23.1+!russia+moscow,!56.43.23.1 - доступ будет дан всем странам, кроме жителей России (за исключением Москвы), а также будет заблокирован пользователь с указанным IP
			// !opera - данная зона не будет доступна для браузеров с UA OPERA
			// Если вы хотите сделать специальную презентационную страничку, доступную только посетителям, которые пришли к вам с Яндекса, то у основной зоны стоит задать параметр !'yandex.ru', а у презентационной зоны 'yandex.ru' соответственно.
			// Если вы хотите, чтобы пользователь попадая с поисковиков по запросу 'Сапоги' попадал на совсем отдельную страничку, стоит указать во всех зонах !'сапоги' а в этой 'сапоги'		
		
		if(!empty($zone[$za]["zone_iprange"])){
			if(empty($_SERVER["REMOTE_ADDR"])){$bool=true; continue;}
			$ip=$user->ip;//$_SERVER["REMOTE_ADDR"];
			$ir=$zone[$za]["zone_iprange"];
			$res=check_ip($ir,$ip);
			if(!$res){$restrict[$za]=1; $bool=true; continue;}
		}
	}
	
	// Если был осуществлён доступ с авто-поддомена - запускаем обработку URL в соответствии с указанными настройками зоны
	if($za!=-3 && !empty($add_to_url)){
		// тип 1 - добавляем имя поддомена в конец URL
		if($zone[$za]["zone_autosub"]==1){
			$ruri.=implode('/',$add_to_url).'/';
			if(empty($_SERVER["REQUEST_URI"])) $_SERVER["REQUEST_URI"]='/'.implode('/',$add_to_url).'/';
			else {
				if(strstr($_SERVER["REQUEST_URI"],'?')){
					$req=explode('?',$_SERVER["REQUEST_URI"]);
					$_SERVER["REQUEST_URI"]=$req[0];
					$req[1]='?'.$req[1];
				} else {$req=Array(); $req[1]='';}
				if($_SERVER["REQUEST_URI"][strlen($_SERVER["REQUEST_URI"])-1]!='/') $_SERVER["REQUEST_URI"].='/'.implode('/',$add_to_url).'/';
				else $_SERVER["REQUEST_URI"].=implode('/',$add_to_url).'/';
				$_SERVER["REQUEST_URI"].=$req[1];
			}
			foreach($add_to_url AS $atu) $ruar[]=$atu;
			$_SERVER["HTTP_HOST"]=substr($_SERVER["HTTP_HOST"],strlen(implode('.',$add_to_url))+1);
			$GLOBALS["auto_subdomain"]=implode('.',$add_to_url);
		}
		// тип 2 - заносим имя поддомена в специальную глобальную переменную
		if($zone[$za]["zone_autosub"]==2){
			$_SERVER["HTTP_HOST"]=substr($_SERVER["HTTP_HOST"],strlen(implode('.',$add_to_url))+1);
			$GLOBALS["auto_subdomain"]=implode('.',$add_to_url);
		}
	}
	unset($restrict);
	
	// Если по каким-то причинам, зона не определена, то запускаем CRON (если это требуется) и выходим
	if($za==-3 && !isset($skip_zone_check)){
		if(!empty($_GET["cron"])) include('core/cron.php');
		if($statistics) collect_stat(false);
		etf('zone_detect');
		exit;
	}
	
	// Если в настройках папки зоны был использован символ *, устанавливаем префикс зоны и переопределяем папку зоны	
	if($za!=-3 && !empty($zone[$za]["zone_folder"]) && $zone[$za]["zone_folder"][0]=='*'){
		$zp=''; if(!empty($GLOBALS["zone_prefix"])){
			$zp=$GLOBALS["zone_prefix"].'/';
		}
		$zone[$za]["zone_folder"]=$zp.substr5($zone[$za]["zone_folder"],1);
	}
	$zone_folder=$zone[$za]["zone_folder"];
	
	// Если у зоны указан email - переопределяем системный email
	if(!empty($zone[$za]["zone_email"])){
		$system_email=$zone[$za]["zone_email"];
		$direct_email=1;
	}
	
	// Вывод отдельного robots.txt у зоны-зеркала
	if(strstr($_SERVER["REQUEST_URI"],'/robots.txt') && !empty($zone[$za]["zone_robots"])){
		header('Content-type: text/plain');
		echo $zone[$za]["zone_robots"];
		if($statistics) collect_stat(false);
		etf('zone_detect');
		exit;
	}
	
	// Обработка редиректов зоны
	if($zone[$za]["zone_redirect"]!=0){
		$nz=-1;
		// в любом случае выполняем шаблон из zone_tpl
		if(!empty($zone[$za]["zone_tpl"])) echo shell_tpl($zone[$za]["zone_tpl"]);
		for($i=0;$i<count($zone);$i++) if($zone[$i]["zone_active"]==1){
			if($zone[$i]["zone_id"]==$zone[$za]["zone_redirect"]) $nz=$i; // простое переопределение зоны
			else if($zone[$i]["zone_id"]==-$zone[$za]["zone_redirect"]){
				// 303ий редирект
				$hh=$zone[$i]["zone_domain"];
				if($hh=='') $hh=$_SERVER["HTTP_HOST"];
				$url=$prot.'://'.$hh;
				if(!empty($zone[$i]["zone_folder"])) $url.='/'.$zone[$i]["zone_folder"];
				$ruri2=$_SERVER["REQUEST_URI"];
				if(!empty($zone[$za]["zone_folder"])) $ruri2=substr5($ruri2,strlen5($zone[$za]["zone_folder"])+1);
				$url.=$ruri2;
				header("HTTP/1.1 301 Moved Permanently"); 
				header("Location: ".$url); 
				if($statistics) collect_stat(false);
				etf('zone_detect');
				exit;
			}
		}
		if($nz==-1){
			header('Content-type: text/plain');
			if(!empty($_GET["cron"])) include('core/cron.php');
			if($statistics) collect_stat(false);
			etf('zone_detect');
			exit;
		}
		$za=$nz;
		// переопределение системного email-а
		if(!empty($zone[$za]["zone_email"]) && !isset($direct_email)) $system_email=$zone[$za]["zone_email"];
	}
	
	// Вызываем шаблон zone_tpl (дублируется для корректной обработки редиректа зоны)
	if(!empty($zone[$za]["zone_tpl"])) echo shell_tpl($zone[$za]["zone_tpl"]);
	
	// Принудительный вывод robots.txt у зоны (дублируется для корректной обработки редиректа зоны)
	if(strstr($_SERVER["REQUEST_URI"],'/robots.txt') && !empty($zone[$za]["zone_robots"])){
		if($statistics) collect_stat(false);
		echo $zone[$za]["zone_robots"];
		etf('zone_detect');
		exit;
	}
	etf('zone_detect');
	
	// Установка глобальных переменных
	stf('set_globals');
	$zi=$zone[$za]["zone_id"];
	$GLOBALS["za"]=$za;
	$GLOBALS["zi"]=$zi;
	if(!empty($base_root)){
		$_SERVER["HTTP_HOST"].=$base_root;
	}
	$GLOBALS["zone_url"]=$prot.'://'.$_SERVER["HTTP_HOST"];
	$GLOBALS["zone_url2"]=$prot.'://'.$_SERVER["HTTP_HOST"].'/'.$zone[$za]["zone_folder"];
	if($zone[$za]["zone_folder"]!='') $GLOBALS["zone_url"]=$GLOBALS["zone_url"].'/'.$zone[$za]["zone_folder"];
	if($GLOBALS["zone_url"][strlen($GLOBALS["zone_url"])-1]=='/') $GLOBALS["zone_url"]=substr($GLOBALS["zone_url"],0,strlen($GLOBALS["zone_url"])-1);

	// Установка адресов HOSTs
	$_SERVER["HTTP_HOST2"]=$_SERVER["HTTP_HOST"];
	$GLOBALS["host6"]=$prot.'://'.$_SERVER["HTTP_HOST"];
	if(!empty($zone[$za]["zone_folder"])) $_SERVER["HTTP_HOST"].='/'.$zone[$za]["zone_folder"];
	$GLOBALS["host"]=$prot.'://'.$_SERVER["HTTP_HOST"].'/';
	$GLOBALS["host2"]=$prot.'://'.$_SERVER["HTTP_HOST"];
	if(!empty($zone[$za]["zone_folder"])) $GLOBALS["host4"]=$zone[$za]["zone_domain"].'/'.$zone[$za]["zone_folder"];
	else $GLOBALS["host4"]=$zone[$za]["zone_domain"];
	
	// Установка прочих переменных
	$GLOBALS["lvl"]=1; // текущий уровень вложения шаблонов
	$GLOBALS["is_admin"]=0; // является ли текущая зона административным кабинетом
	$GLOBALS["sforward"]=$prot.'://'.$_SERVER["HTTP_HOST2"].$_SERVER["REQUEST_URI"]; //url (для URL-forward функций шаблона) по умолчанию
	
	// Проверка на запуск из IFrame или XSS
	$cross_site=((!empty($_POST) || !empty($_GET)) && !empty($_SERVER["HTTP_REFERER"]) && !strstr($_SERVER["HTTP_REFERER"],$_SERVER["HTTP_HOST2"]));
	
	// Генерация путей и доп.переменных
	$ict=its_cron_time(); // настало ли время запуститься CRON-у?
	$xurl=str_replace($prot.'://'.$_SERVER["HTTP_HOST"],'',$GLOBALS["zone_url"]);		
	$xpath=explode('?',$_SERVER["REQUEST_URI"]);
	$xurl=substr($xpath[0],strlen($xurl+1));
	$xurl=str_replace("'",'`',$xurl);
	$xurl=urldecode($xurl);
	if(!empty($xurl)) if($xurl[strlen($xurl)-1]=='/') $xurl=substr($xurl,0,strlen($xurl)-1);
	//$GLOBALS["g_url"]=$xurl;
	$GLOBALS["fullurl"]=$GLOBALS["host"].$xurl;
	$GLOBALS["forward"]=$prot.'://'.$_SERVER["HTTP_HOST2"].$_SERVER["REQUEST_URI"];
	$GLOBALS["fullurl2"]=$GLOBALS["forward"];
	$url=Array();
	$GLOBALS["relative"]=$base_root;
	if(!empty($zone_folder)){
		if(!strstr($xurl,$zone_folder.'/')) $xurl=str_replace($zone_folder,'',$xurl);
		else $xurl=str_replace($zone_folder.'/','',$xurl);
		if(!empty($base_root)) $GLOBALS["relative"].='/';
		$GLOBALS["relative"].=$zone_folder;
	}
	$GLOBALS["g_url"]=$xurl;
	if($xurl=='') $url[0]=''; else $url=explode('/',$xurl);
	
	// Игнорируем безопасное соединение для Ajax из административной части
	$allow_safe=true;
	if($zone[$za]["zone_module"]==-1 && strstr($GLOBALS["g_url"],'/ajax')) $allow_safe=false;
	
	// Загрузка прав доступа
	include('core/units/perm.inc');	
	if(!$user->super){$GLOBALS["password"]='';$GLOBALS["username"]='';}
	etf('set_globals');
	
	// проверка на наличие прав доступа для определённой зоны
	
	if(!check_zone($zi,'view') && !isset($skip_zone_check)){
		if(!empty($_GET["exsys5"]) && !empty($_GET["ajax"])){
			if($statistics) collect_stat(false);
			include('core/ajax.inc'); // загрузка аякс-запроса (ajax) или iframe-запроса (exsys5)
			exit;
		} else {
			// форма авторизации
			if($statistics) collect_stat(false);
			if(!empty($_GET["cron"])) include('core/cron.php');
			if(!empty($GLOBALS["auth_err"]) && $GLOBALS["auth_err"]==1) $GLOBALS["auth_err"]="Пользователю запрещён вход в данную зону";
			include('core/deny.inc');
			exit;
		}
	}
	
	// Этап 4. Запуск определённой зоны
	
	// Запуск CRON
	if(!empty($_GET["cron"]) && $GLOBALS["cron_type"]==0){
		if($statistics) collect_stat(false);
		include('core/cron.php');
		exit;
	}
	
	// Запуск Ajax
	if(!empty($_GET["ajax"])){
		if($statistics) collect_stat(false);
		if($check_for_xss) check_xss();
		include('core/ajax.inc');
		exit;
	}
	
	// Запрос обновления
	if($zone[$za]["zone_module"]==-2){
		if($statistics) collect_stat(false);
		include('core/update/server.php');
		exit;
	}		
	
	//Защита от возможного двойного запуска головного файла (например из CRON или Ajax части)
	$dbg=debug_backtrace();
	if(count($dbg)==0){
	
	// Административная часть
	if($zone[$za]["zone_module"]==-1){	
	
		// Выход пользователя
		if(!empty($_GET["action"]) && $_GET["action"]=='user_exit'){
			if($statistics) collect_stat(false);
			include('core/deny.inc');
			exit;
		}
		// Отгрузка данных для тестирования соединения
		if(!empty($_GET["testings"]) && $_GET["testings"]==1 && !$cross_site){
			if($statistics) collect_stat(false);
			start_timer();
			$gc=get_code2(1024*1024*2);//передаём 2мб клиенту
			header("Content-Encoding: identity");//не сжимаем
			echo '<html><script>var d = new Date(); var x=d.getSeconds()+\'.\'+d.getMilliseconds(); </script><body OnLoad="var d2 = new Date(); var y=d2.getSeconds()+\'.\'+d2.getMilliseconds(); document.location.href=\'settings?testings=2&x=\'+(y-x)+\'&y=\'+'.end_timer().';">Проводится тестирование скорости отдачи сервера. Подождите несколько секунд...<span style="display: none;">'.$gc.'</span></body></html>';
			exit;
		}
					
		// Вход в административную зону
		if(!$cross_site){
			$GLOBALS["is_admin"]=1;
			$pdz=$zone[$za]["zone_safe"];
			if($pdz==2 && $allow_safe) start_safe_output(); // если предполагается шифрование данных - включаем его
			add_magic_quotes(); // добавляем обработку кавычек для входящих данных
			include('core/editor/index.php');
			if($pdz==2 && $allow_safe) stop_safe_output(); // отключаем шифрование данных
		}
		
		if($statistics) collect_stat(false);
				
		exit;
		
	// Клиентская часть сайта (внешнаяя)
	} else {
		if($check_for_xss) check_xss(); // проверка на XSS
		// очистка от get_magic_quotes
		// clean_magic_quotes(); (уже есть выше)
		// запускаем обработку аппендикса URL в указанный модуль
		$GLOBALS["last_cow"]=0; // самый глубокий объект, участвующий в URL-разборе		
		$res=prepend_url($zone[$za]["zone_module"],$url);
		// получаем текущую часть модуля и текущий объект (если есть), сопоставленный с URL
		if(!empty($res) && !empty($res->part)){
			// устанавливаем информацию о результате подбора части в глобальные переменные
			$GLOBALS["cur_row"]=$res->row;
			$GLOBALS["cur_part"]=$res->part;
			$GLOBALS["cur_ex"]=$res->module_ex;
			$GLOBALS["cur_module"]=$res->module_id;
			$GLOBALS["cur_table"]=$res->table_id;
			$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
			getrow($db,"SELECT * FROM main_part WHERE part_id=".$res->part,1,'main_part');
			$pbody=$db->Record["part_body"];
			$pparse=$db->Record["part_parse"];
			$GLOBALS["is_404"]=0; if(isset($GLOBALS["last_404"])){$GLOBALS["is_404"]=1;unset($GLOBALS["last_404"]);}
			if(!empty($db->Record["part_ifrow"])){
				$GLOBALS["cur_row"]=parse_var($db->Record["part_ifrow"]);
				$res->row=$GLOBALS["cur_row"];
			}			
			
			// передаём код текущей части на исполнение в шаблонизатор
			if($pparse==0) $echo=shell_tpl($pbody);
			if($pparse==1) $echo=shell_php($pbody);
			
			// проверка на наличие обработчика 404 ошибки
			$r=check_404_page($res,$zone[$za]["zone_module"]);
			if(!empty($r)) $echo=$r['data'];
			
			// если мы имеем дело с html - отправляем заголовок о кодировке (если это требуется)
			if(!empty($def_charset) && strpos(strtolower($echo),'<html>')!==false){
				header('Content-type: text/html; charset="'.$def_charset.'"');
				if(!isset($GLOBALS['tpl_header'])) $GLOBALS['tpl_header']=Array();
				$GLOBALS['tpl_header'][]='Content-type: text/html; charset="'.$def_charset.'"';
			}
			
			// если существует неопределённый аппендикс URL, отправляем 404 заголовок (если это требуется)
			if(!empty($show_404) && !empty($GLOBALS['min_url'])){
				header('HTTP/1.0 404 Not Found');
				$is_404=true;
				if(!isset($GLOBALS['tpl_header'])) $GLOBALS['tpl_header']=Array();
				$GLOBALS['tpl_header'][]='HTTP/1.0 404 Not Found';
			}
			
			// подключаем сгенерированные файлы CSS и JS через команду COMPILE
			global $compile,$head;
			if(!empty($compile)){
				if(isset($compile['less'])){
					include_once(DOCUMENT_ROOT.'/core/units/left/lessc.php');
					$less = new lessc;
					$data=$less->compile(implode(VSP2,$compile['less']));
					if(!isset($compile['css'])) $compile['css']=Array(); else $data=VSP2.$data;
					$compile['css'][]=$data;
					unset($compile['less']);
				}
				foreach($compile AS $type=>$data){
					$data=implode(VSP2,$data);
					$url=/*$base_root.*/CTEMP.md5($data).'.'.$type;
					if(empty($head)) $head='';
					$compile_error=0;
					if(!file_exists(DOCUMENT_ROOT.$url)){
						check_dir($base_root.CTEMP);
						if(!file_put_contents(DOCUMENT_ROOT.$url,$data)){
							if($type=='css') $head.='<style>'.$data.'</style>';
							if($type=='js') $head.='<script>'.$data.'</sript>';
							$compile_error=1;
						}
					}
					if($compile_error==0){
						if($type=='css') $head.='<link href="'.$url.'" rel="stylesheet">'.VSP2;
						if($type=='js') $head.='<script src="'.$url.'"></script>'.VSP2;
					}
				}
			}
			
			// если в результате выполнения шаблона возникла необходимость добавить контент в тег <head> - делаем это
			if(!empty($head)){
				$a=strpos(strtolower($echo),'</head>');
				if($a){
					if(!empty($GLOBALS["sp_head"]) && isset($GLOBALS["xn_user_protect_type"]) && in_ext($_SERVER["REQUEST_URI"],Array('png','jpg','gif','jpeg','css','js','ico'))){
						$a=false;
					} else {
						//ищем отступ
						$step='';
						for($i=1;$i<200;$i++) if(!empty($echo[$a-$i])){
							if(ord($echo[$a-$i])==10) break;
							else $step=$echo[$a-$i].$step;
						}
						$echo=substr5($echo,0,$a).cascading_html($head,strlen($step)+1,0).chr(10).$step.substr5($echo,$a);
					}
				}
				if(!$a){
					if(!empty($GLOBALS["sp_head"]) && isset($GLOBALS["xn_user_protect_type"])){
						$GLOBALS["zone_deny"]=$GLOBALS["e5uid"];
						$x=explode('|-|-|',$GLOBALS["xn_user_protect_old"]);
						SetCookie("z-auth",$x[1],0,'/','',false);
						if($GLOBALS["xn_user_protect_type"]==-1){
							$su_safe=$GLOBALS["xn_user_protect_old"];
							//save_config();
							save_timing();
						} else {
							global $db;
							$db->query("UPDATE main_auth SET user_safe='".$GLOBALS["xn_user_protect_old"]."' WHERE auth_id=".$GLOBALS["xn_user_protect_type"],3,"main_auth");
						}
					}
				}
			}
			// если предполагается дальнейший запуск CRON - открываем новый буфер вывода
			if($ict){
				header("Connection: close");
				ob_start();
			}
			if($zone[$za]["zone_safe"]==2) start_safe_output(); // если предполагается шифрование контента - шифруем
			echo $echo; // вывод контента
			if($zone[$za]["zone_safe"]==2) stop_safe_output(); // заканчиваем шифрование
			// жёсткое кеширование
			if($hard_cache && empty($is_404)){
				check_dir(DOCUMENT_ROOT.'/core/cache/h');
				$f=fopen(DOCUMENT_ROOT.'/core/cache/h/'.$uri_md5,'w');
				fwrite($f,$echo);
				fclose($f);
				//if(!empty($GLOBALS['tpl_static'])){
				//	$f=fopen(DOCUMENT_ROOT.'/core/cache/h/'.$uri_md5.'_static','w');
				//	fwrite($f,$GLOBALS['tpl_static']);
				//	fclose($f);
				//}
				if(!empty($GLOBALS['tpl_header'])){
					$f=fopen(DOCUMENT_ROOT.'/core/cache/h/'.$uri_md5.'_static','w');
					fwrite($f,serialize($GLOBALS['tpl_header']));
					fclose($f);
				}
			}
		}
	}

	// Статистика
	if($statistics) collect_stat();

	// Debug / Отладка
	if(($user->ip==$ip_debug || $ip_debug=='') && $debug && !strpos($_SERVER["REQUEST_URI"],'.css') && !strpos($_SERVER["REQUEST_URI"],'.jpg') && !strpos($_SERVER["REQUEST_URI"],'.gif') && !strpos($_SERVER["REQUEST_URI"],'.png') && !strpos($_SERVER["REQUEST_URI"],'.xml')){
		include('core/units/debug.inc');
	}
	}
	
	// Запуск CRON с помощью посетителя
	if($ict && !isset($skip_zone_check)){
		$size=ob_get_length();
		@header("Content-Length: $size");
		if($size>0) ob_end_flush();
		flush();
		// в этот момент пользователь отсоединяется от сервера
		// в то время как сервер продолжает обрабатывать запрос
		ob_start();
		include('core/cron.php');
		ob_end_clean();
	}
	
?>