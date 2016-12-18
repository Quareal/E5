[*

Переменные

$title				заголовок страницы
$base_root		путь до установленной системы (если система была установлена в подпапку - путь до конечной папки с системой), пример эксплуатации: [$base_root]/files/editor/style.css
$zone_root		URL до административной панели, с учётом домена, не оканчивается на слеш, пример значения - http://site.ru/admin
glob.use_jquery	сообщение плагинам о том, что вы подключили в шаблон JQuery (для этого установите это значение в 1, пример - [glob.use_jquery=1])
$head			блок заголовка, который может быть сгенерирован плагинами (должен находиться между <head> и </head>)
$mail_url			путь до почтового ящика пользователя
$mail_cnt			количество новых писем пользователя
$title_conf			ссылка на настройки текущей страницы (например, настройки текущей таблицы)
$view_zone		возможность просматривать страницу с зонами (адрес: /zones)
$senter			возможность просматривать страницу с статистикой (адрес: /statistics)
$genter			возможность просматривать страницу с группами пользователей (адрес: /group)
$user_url			URL до редактирования профиля пользователя (если редактирование текущего пользователя недоступно - содержит пустую строку)
$first_uname		первая буква логина пользователя (суперпользователь называется SuperUser)
$last_uname		остальные буквы логина пользователя
$debug			состояние дебагинга (true - включён, false - выключен)
$load_average		средняя загруженность сервера (параметр load average)
$msize			объём свободной оперативной памяти на сервере (в мегабайтах)
$tmsize			суммарный размер ОЗУ, доступной на сервере (если информация доступна, в мегабайтах)
$tquery			количество запросов к БД
$ttime			время генерации страницы (в секундах, точность до 1/1000 секунды)

Пути

/modules			перечень доступных модулей (не требуется специальных разрешений)
/zones			перечень сайтов, доступных в системе (для проверки доступа используйте переменную $view_zone)
/group			настройки доступа (проверка доступа по переменной $genter)
/statistics			статистика посещаемости (проверка доступа по переменной $senter)
/update			раздел обновлений (треубет доступ суперпользователя, проверить его можно через user.super?)
/settings			страница настроек (требует доступ суперпользователя)
/terminal			терминал (требует доступ суперпользователя)
/mail				почтовый ящик
/news			раздел новостей пользователя

Компоненты

include.editor.main				Основное содержимое административного кабинета
include.editor.parts.breadcrumbs	Компонент "хлебные крошки" с раскрывающейся навигацией
include.editor.parts.zone_select		Компонент "выбор сайта" для соответствующего виджета
include.editor.parts.news			Компонент "вывод новостей" пользователя

*]<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title>[$title.clear]</title>
	
	<meta http-equiv="Pragma" content="no-cache" />
	<meta http-equiv="Cache-Control" content="no-cache" />
	<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
	<meta http-equiv="content-style-type" content="text/css2" />
	
	<link rel="icon" type="image/png" href="[$base_root]/files/editor/classic/logo2.png">
	<link rel="shortcut icon" href="[$base_root]/files/editor/icon.png">
	<meta name="yandex-tableau-widget" content="logo=[$base_root]/files/editor/classic/logo.png, color=#3072C9" />
	
	<link rel="stylesheet" type="text/css" href="[$base_root]/files/editor/classic/styles.css" title="default" />
	<link rel="stylesheet" type="text/css" href="[$base_root]/files/editor/classic/tables.css" title="default" />
	
	<script>var jsq="[$base_root]"; var jsq2="[$zone_url]";</script>
	
	<script type="text/javascript" src="[$base_root]/files/editor/jquery-2.1.4.min.js"></script>[glob.use_jquery=1]	
	<script type="text/javascript" src="[$base_root]/files/editor/script.js"></script>
	<script type="text/javascript" src="[$base_root]/files/js/elfinder/js/jquery-ui-1.7.2.custom.min.js"></script>[glob.use_jquery_ui=1]	
	<script type="text/javascript" src="[$base_root]/files/editor/jquery.tablednd_0_5.js"></script>
	
	<link rel="stylesheet" type="text/css" href="[$base_root]/files/js/elfinder/js/ui-themes/base/ui.all.css" title="default" />
	<link rel="stylesheet" type="text/css" href="[$base_root]/files/js/elfinder/css/elfinder.css" title="default" />
	<script type="text/javascript" src="[$base_root]/files/js/elfinder/js/elfinder.min.js"></script>
	<script type="text/javascript" src="[$base_root]/files/js/elfinder/js/i18n/elfinder.ru.js"></script>
	
	<link rel="stylesheet" type="text/css" href="[$base_root]/files/editor/meditor/styles.css" title="default" />
	<script type="text/javascript" src="[$base_root]/files/editor/meditor/script.js"></script>
	<script type="text/javascript" src="[$base_root]/files/editor/meditor/insert-manager.js"></script>		
	
	<script src="[$base_root]/files/editor/ace/ace.js" type="text/javascript" charset="utf-8"></script>
	
	[if $head]
	[$head][/if]
</head>
<body[if !$no_design] onLoad="MM_preloadImages('[$base_root]/files/editor/classic/m11.png','[$base_root]/files/editor/classic/m22.png','[$base_root]/files/editor/classic/m33.png','[$base_root]/files/editor/classic/m44.png','[$base_root]/files/editor/classic/m55.png')"[/if]>
[if !$no_design]
<div id="menu_shadow" OnClick="hidewnd();" style="display: none;" class="shadow">&nbsp;</div>
<div id="menu" style="display: none;" class="menu_cont">
	<img src="[$base_root]/files/editor/classic/btop.png" width="200" height="14">
	<div id="mcontent" class="menu_block"></div>
	<img src="[$base_root]/files/editor/classic/bbot.png" width="200" height="7">
</div>
<div style="position: fixed; left: 2px; top: 9px; display: none;" id="expander"><img src="[$base_root]/files/editor/classic/arrow.png" width="12" height="9" border="0" align="absmiddle" style="cursor: pointer;" OnClick="$('#expander').hide(); $('.left').each(function(){$(this).show();});"></div>
<table width="100%" height="100%" cellpadding="0"  cellspacing="0">
	<tr>
		<td class="left" width="194" height="64" bgcolor="#1076DC">
			<table width="194" height="64" cellpadding="0"  cellspacing="0">
				<tr>
					<td width="75" valign="top">
						<div style="margin-top: 5px; margin-left: 5px;"><img src="[$base_root]/files/editor/classic/arrow-left.png" width="12" height="9" border="0" align="absmiddle" style="cursor: pointer;" OnClick="$('.left').each(function(){$(this).hide();}); $('#expander').show(); "></div>
					</td>
					<td width="48" valign="bottom"><a href="[$zone_url]"><img src="[$base_root]/files/editor/classic/logo.png" width="48" height="36" border="0" style="margin-bottom: 7px;"></a></td>
					<td align="right" valign="bottom">
						<div style="padding-right: 5px; padding-bottom: 3px; color: #FFFFFF;">
							<a href="[$mail_url]" class="awhite">[$mail_cnt] <img src="[$base_root]/files/editor/classic/msg.png" width="16" height="13" border="0" align="absmiddle"></a>
						</div>
					</td>
				</tr>
			</table>
		</td>
		<td width="8" rowspan="7"></td>
		<td valign="bottom" bgcolor="#1076DC">
			<div style="float: left; font-size: 18px; color: #FFFFFF; padding-left: 15px; padding-top: 39px;">
				[$title]
				[if $title_conf]<a href="[$title_conf]"><img src="[$base_root]/files/editor/classic/cfg.png" width="25" height="23" hspace="3" border="0" align="absmiddle" style="margin-bottom: 1px;"></a>[/if]
			</div>
			<div style="float: right; width: 500px;" align="right">
				[if $view_zone]<a href="[$zone_url]/zones" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image6','','[$base_root]/files/editor/classic/m44.png',1)"><img src="[$base_root]/files/editor/classic/m04.png" name="Image6" width="85" height="64" border="0"></a>[/if][if $senter]<a href="[$zone_url]/statistics" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image15','','[$base_root]/files/editor/classic/m66.png',1)"><img src="[$base_root]/files/editor/classic/m06.png" name="Image15" width="82" height="64" border="0"></a>[/if][if $genter]<a href="[$zone_url]/group" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image5','','[$base_root]/files/editor/classic/m33.png',1)"><img src="[$base_root]/files/editor/classic/m03.png" name="Image5" width="82" height="64" border="0"></a>[/if]<a href="[$zone_url]/modules" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image4','','[$base_root]/files/editor/classic/m22.png',1)"><img src="[$base_root]/files/editor/classic/m02.png" name="Image4" width="83" height="64" border="0"></a>[if user.super?]<a href="[$zone_url]/update" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image13','','[$base_root]/files/editor/classic/m55.png',1)"><img src="[$base_root]/files/editor/classic/m05.png" name="Image13" width="82" height="64" border="0"></a>[/if][if user.super?]<a href="[$zone_url]/settings" onMouseOut="MM_swapImgRestore()" onMouseOver="MM_swapImage('Image3','','[$base_root]/files/editor/classic/m11.png',1)"><img src="[$base_root]/files/editor/classic/m01.png" name="Image3" width="86" height="64" border="0"></a>[/if]
			</div>
		</td>
	</tr>
	<tr>
		<td class="left" height="4"></td>
		<td height="4"></td>
	</tr>
	<tr>
		<td class="left" height="18" valign="middle" bgcolor="#FFFFFF" style="font-weight: bold; font-size: 11px;">
			<div style="float: left; overflow: hidden; height: 16px; width: 168px;">
				<nobr>
					<a href="[$zone_url]?action=user_exit"><img src="[$base_root]/files/editor/classic/exit.png" width="16" height="16" hspace="5" border="0" align="absmiddle"></a>
					[if $user_url]<a href="[$user_url]" class="user"><span class="red">[$first_uname]</span>[$last_uname]</a>[else]<span class="user"><span class="red">[$first_uname]</span>[$last_uname]</span>[/if]
				</nobr>
			</div>
			<div style="float: right; width: 26px;"><img src="[$base_root]/files/editor/classic/file.png" width="14" height="14" hspace="6" border="0" OnClick="show_elfinder();" style="cursor: pointer;"></div>
		</td>
		<td height="18" valign="middle" bgcolor="#FFFFFF"><div style="padding-left: 16px;">
			[include.editor.parts.breadcrumbs]
		</div></td>
	</tr>
	<tr>
		<td class="left" height="4"></td>
		<td height="4"></td>
	</tr>
	<tr>
		<td class="left" valign="top">
			[include.editor.parts.edit_parts]
			[include.editor.parts.zone_select]
			[include.editor.parts.news]
		</td>
		<td valign="top">
			<table width="100%" cellpadding="0" cellspacing="0">
				<tr>
					<td class="content_td">
[/if]
						[include.editor.main]
[if !$no_design]
					</td>
				</tr>
				<tr>
					<td height="8"></td>
				</tr>
				<tr>
					<td height="33" bgcolor="#1076DC">
						<table width="100%" height="33" cellpadding="0" cellspacing="0">
							<tr>
								[include.editor.parts.measure]
								<td width="16">&nbsp;</td>
								<td valign="middle">
									[if user.super?]<a href="[$zone_url]/terminal"><img src="[$base_root]/files/editor/classic/terminal.png" style="cursor: pointer; margin-top: 4px;"></a>[else]&nbsp;[/if]
								</td>
								<td width="56" valign="middle" align="left">
									[if $debug]<img src="[$base_root]/files/editor/classic/debug.png" OnClick="showhide('debug_view'); scroll(0,9999);" style="cursor: pointer; margin-top: 3px;">[/if]
								</td>
	    							[if $load_average]<td width="70" valign="middle" align="left" class="white normal"><div style="padding-top: 3px;"><img src="[$base_root]/files/editor/classic/cpu.png" align="absmiddle" style="margin-right: 5px; margin-top: 0px;">[$load_average]</div></td>[/if]
	    							<td width="[if $tmsize]120[else]70[/if]" valign="middle" align="left" class="white normal"><div style="padding-top: 3px;"><img src="[$base_root]/files/editor/classic/memory.png" align="absmiddle" style="margin-right: 5px; margin-top: 0px;"> [$msize]<span style="font-size: 10px;">m</span>[if $tmsize]<span style="font-size: 10px; color: #81B6EA;"> / [$tmsize]</span>[/if]</div></td>
								<td width="88" valign="middle" align="left" class="white normal"><div style="padding-top: 4px;"><img src="[$base_root]/files/editor/classic/sql.png" align="absmiddle" style="margin-right: 5px; margin-top: 0px;"><b>SQL</b> [$tquery]</div></td>
								<td width="98" valign="middle" align="left" class="white normal"><div style="padding-top: 4px;"><img src="[$base_root]/files/editor/classic/time.png" align="absmiddle" style="margin-right: 5px; margin-top: 0px;">[$ttime]</div></td>
							</tr>
						</table>
					</td>
				</tr>
			</table>
		</td>
	</tr>
</table>
[if $debug]
	<div style="padding-top: 20px; display: none;" id="debug_view">			
		[include.editor.debug]
	</div>
[/if]
[/if]
<div id="finder" style=""></div>
</body>
</html>