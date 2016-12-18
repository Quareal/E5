<?php

echo '<script>
var old_part_url="";
var old_table=0;
function url_select(value){
	if(value!=2 && value!=0 && $("#part_table").val()){
		old_table=$("#part_table").val();
		$("#part_table").val(0);
	} else if(old_table && $("#part_table").val()==0){
		$("#part_table").val(old_table);
	}
	if(value==2){
		$("#part_table_link").appendTo("#place-for-table");
	} else  {
		$("#part_table_link").appendTo("#base-place-for-table");
	}
	if(value==0 || value==4 || value==5){
		if(value==4){
			old_part_url=$("#save_url").val();
			$("#save_url").attr("value","");
		}
		$("#type1c2").hide();
	} else {
		if(old_part_url && !$("save_url").val()){
			$("#save_url").attr("value",old_part_url);
			old_part_url="";
		}
		$("#type1c2").show();
		$("#part_iowner").hide();
		$("#part_sowner").hide();
		$("#part_url").hide();
		$("#part_ifdetect").hide();
		if(value==1){
			$("#part_url").show();			
		}
		if(value==2){
			$("#part_url").show();
			$("#part_iowner").show();
			if(!$("#part_iowner_check").attr("checked")) $("#part_sowner").show();
		}
		if(value==3){
			$("#part_url").show();
			$("#part_ifdetect").show();
		}
	}
}
</script>';

//=======================
//  Вывод частей модуля
//=======================

function sia_part($parts,$owner,$id){
	if(empty($parts[$owner])) return false;
	foreach($parts[$owner] AS $prt){
		if($prt["part_id"]==$id) return true;
		if(sia_part($parts,$prt["part_id"],$id)) return true;
	}
	return false;
}

$first_list_part=true;
function echo_parts($type,$sql,$title='',$owner=0){//type: 1 - url
	global $id,$c_ex,$id2,$action,$aex,$db,$gparts,$first_list_part,$lth;
	
	$sqlC=$sql;	
	if($owner!=0)$sqlD="SELECT * FROM main_part WHERE part_owner=$owner ORDER BY part_url, part_name";
	else $sqlD=$sql;
	$parts=getall($db,$sqlD);
	if(empty($parts)) return '';
	if($owner==0){
		$h='<h2>'.$title.'</h2>';		
		if($first_list_part){
			//поле поиска
			echo '<br><table cellpadding="0" cellspacing="0" width="100%"><tr><td valign="center" align="left">'.$h.'</td>
			<td align="right" valign="center" width="310">
				<form action="mod_main" method="post" style="margin: 0px; padding: 0px;">
					<input type="hidden" name="id" value="'.$id.'">
					<input type="hidden" name="action" value="part_search">
					'.si('search').'
					<input type="text" name="query" value="" style="width: 250px;">
					<input type="submit" value=">" class="arrow_btn">
				</form>
			</td></tr></table>';
		} else echo '<br>'.$h;
		$first_list_part=false;
		echo '<table id="records" cellpadding="3" cellspacing="1">';
		$lth='<tr>';
		$lth.='<th>Название</th>';
		if($type==1) $lth.='<th>URL</th>';
		if($type==2){
			$lth.='<th>Раздел</th>';
			$lth.='<th>Автозапуск</th>';
		}
		foreach($parts AS $p){
			$tp=$p["part_type"];
			break;
		}
		if($tp==4) $lth.='<th>Раздел</th>';
		if($tp==0 || $tp==1 || $tp==5) $lth.='<th width="130">Действия</th>';
		else if($tp==3) $lth.='<th width="190">Действия</th>';		
		else if($tp==4) $lth.='<th width="160">Действия</th>';
		$lth.='</tr>';
		echo $lth;
	} else {
		$dsp='none';
		if(($action=='edit_part_form' || $action=='timer_form') && sia_part($parts,$owner,$id2)) $dsp='';
		echo '<tr id="prt'.$owner.'" style="display: '.$dsp.';">';
		if($type==1)  echo '<td colspan="3">';
		else if($type==2)  echo '<td colspan="4">';
		else echo '<td colspan="2">';
		echo '<table id="records" cellpadding="3" cellspacing="1">';
		//echo str_replace('<tr>','<tr id="think-row">',$lth);
	}
	if(!empty($parts)) foreach($parts AS $part){
		$ct=$part;
		echo '<tr>';
		echo '<td>';
		if(!empty($gparts[$ct["part_id"]])) echo '<span class="link" OnClick="showhide(\'prt'.$ct["part_id"].'\');">'.se('tree','','Посмотреть подчасти','',false);
		if(!empty($ct["part_pic"])) echo se('/files/editor/icons/'.$ct["part_pic"],'','','',false);
		echo $ct["part_name"];
		if(!empty($gparts[$ct["part_id"]])) echo '</span>';
		echo ' <span style="color: #777777;">('.$ct["part_sname"];
		if($ct["part_table"]!=0) echo se('url','','Связь с таблицей','',false,0,0,'','margin-left: 5px;').' '.getrow2("SELECT * FROM main_table WHERE table_id=".$ct["part_table"],"table_name");
		echo ')</span>';
		if($ct["part_access"]==1) echo se('global','','Глобальная видимость','',false);
		if($ct["part_auth"]!=-1) echo se('user','','Запуск с определёнными правами доступа','',false);
		echo '</td>';
		if($type==1) {
			echo '<td>'.$ct["part_url"];
			if($ct["part_ifcase"]!='' || $ct["part_ifdetect"]!='') echo se('help','','Есть условие для вызова','',false);
			if($ct["part_ifrow"]!='') echo se('anchor','','Есть переопределение текущего объекта','',false);
			echo '</td>';
		}
		if($type==2 || $ct["part_type"]==4){
			echo '<td>';
			if($ct["part_type"]==3 || $ct["part_type"]==4){
				if($ct["part_ex"]==0 && $c_ex>1){
					echo ($ct["parser_end_ex"]+1).' из '.$c_ex;
				}
				if($ct["part_ex"]==-1){
					echo 'Не связан с разделом';
				}
				if($ct["part_ex"]>0){
					if(empty($aex[$ct["part_ex"]])) echo 'Раздел, с которым связана часть, удалён';
					else echo $aex[$ct["part_ex"]]["ex_name"];
				}
			}			
			echo '</td>';
		}
		if($type==2){			
			echo '<td>';
			if($ct["timer_type"]==0) echo 'нет';
			else if($ct["timer_type"]==1){
				echo 'Разовый запуск<br>';
				if(get_day(date('Y-m-d'))*24*60+get_min(date('H:i'))<get_day($ct["timer_date"])*24*60+get_min($ct["timer_time"])) echo 'будет'; else echo 'был';
				echo ' '.get_normal_date($ct["timer_date"]).'<br>в '.$ct["timer_time"];
			} else if($ct["timer_type"]==2){
				echo 'Запуск через '.get_str_num($ct["timer_x"],'каждую','каждые','каждые').' '.$ct["timer_x"].' '.get_str_num($ct["timer_x"],'минуту','минуты','минут');
			} else if($ct["timer_type"]==3){
				echo 'Запуск каждый день в '.$ct["timer_time"];
			}
			/*if($ct["timer_last"]!='0000-00-00 00:00:00'){
				$da=explode(' ',$ct["timer_last"]);
				$dt=explode(':',$da[1]);
				echo '<br><span style="font-size: 10px; color: #888888;">Последний запуск: '.get_normal_date($da[0]).' в '.$dt[0].':'.$dt[1].'</span>';
			} else if($ct["part_shell"]!='0000-00-00 00:00:00'){
				$da=explode(' ',$ct["part_shell"]);
				$dt=explode(':',$da[1]);
				echo '<br><span style="font-size: 10px; color: #888888;">Последний запуск: '.get_normal_date($da[0]).' в '.$dt[0].':'.$dt[1].'</span>';
			}*/ //в принципе эта информация есть, когда я жму на таймер
			echo '</td>';
		}
		echo '<td align="right">';
		$stat='';
		$imp=0;
		if($ct["part_type"]==3){
			echo se('shell','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=shell&amp;key='.get_form_protection_key('mod_main',1,0),'Выполнить');
		}
		if($ct["part_type"]==4){
			$imp=1;
			echo se('shell','','Загрузить файл для импорта','',false,5,0,' OnClick="showhide(\'mm'.$ct["part_id"].'\')"','cursor: pointer;');
		}
		if($ct["part_type"]==3){
			if($ct["timer_type"]==0) echo se('timer','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=timer_form#timer_form','Настройка времени запуска');
			else echo se('timer2','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=timer_form#timer_form','Изменение времени запуска');
		}
		if(check_prt($ct["part_id"],'edit')){			
			global $user;
			if($ct["part_parse"]!=1 || $user->super){
				if($ct["part_parse"]==0) echo se('code','mod_part?id='.$id.'&amp;id2='.$ct["part_id"],'E5 шаблонизатор');
				if($ct["part_parse"]==1) echo se('php','mod_part?id='.$id.'&amp;id2='.$ct["part_id"],'PHP');
				//echo '	<a href="mod_part?id='.$id.'&amp;id2='.$ct["part_id"].'">Содержимое</a> ';
				echo se('back_table','parts_param?id='.$id.'&amp;id2='.$ct["part_id"],'Переменные части');//echo '	<a href="parts_param?id='.$id.'&amp;id2='.$ct["part_id"].'">Переменные</a> ';
				if($ct["part_enable"]==1) echo se('deactivate','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=deactivate_part&amp;key='.get_form_protection_key('mod_main',1,0));
				else echo se('activate','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=activate_part&amp;key='.get_form_protection_key('mod_main',1,0));
				echo se('edit','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=edit_part_form#edit_part');//echo '	<a href="mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=edit_part_form#edit_part">Изменить</a>';
				echo se('del','mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=del_part');//echo '	<a href="mod_main?id='.$id.'&amp;id2='.$ct["part_id"].'&amp;action=del_part">Удалить</a>';
			} else echo 'Доступ разрешён только для супер-пользователя';
		} else echo 'Текущий пользователь не может редактировать эту часть';
		if($imp!=0) echo '<form id="mm'.$ct["part_id"].'" style="display: none;" action="mod_main" method="post" enctype="multipart/form-data"><input type="hidden" name="id" value="'.$id.'"><input type="hidden" name="id2" value="'.$ct["part_id"].'">'.get_form_protection_key('mod_main',1,1).'<input type="hidden" name="action" value="shell"><p><b>Файл</b>:<input type="file" name="part_file"><br><input type="submit" value="Выполнить" class="button"></p></form>';
		echo $stat;
		echo '</td></tr>';
		/*if(!empty($parts[$ct["part_id"]])) */echo_parts($type,/*$parts*/$sqlC,$title,$ct["part_id"]);
	}
	if($owner==0){
		echo '</table>';
	} else {
		echo '</table></td></tr>';
	}
}

$gparts=getall5($db,"SELECT * FROM main_part WHERE part_module=$id","part_owner");
if(check_mod($id,'edit')){
	echo_parts(1,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=0 AND part_ignore=0 ORDER BY part_url, part_name",'Части модуля для обработки URL');
	echo_parts(0,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=0 AND part_ignore=1 ORDER BY part_name",'Вспомогательные части');
	echo_parts(0,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=1 ORDER BY part_name",'Виджеты / Блоки');
	echo_parts(2,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=3 ORDER BY part_name",'Действия / Обработчики');
	echo_parts(0,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=5 ORDER BY part_name",'Действия для строк/рядов');
	echo_parts(0,"SELECT * FROM main_part WHERE part_module=$id AND part_owner=0 AND part_type=4 ORDER BY part_name",'Обработка импорта');
}

$part_help=show_help('Не рекомендуется создавать виртуальные пути в папке Files. Если такая необходимость возникла - измените соответствующим образом файл .htaccess в папке Files');
$part_help2=show_help('Если включён разбор строк из подчинённых таблиц, то необходимо в поле "связь с таблицей" указывать не конечную таблицу объекта-разбора, а вернхюю таблицу самого дальнего родителя. В этом случае части может передаваться объекты из разных подтаблиц, если таковые подходят под URL разбор.');

//=======================
//  Форма добавления части
//=======================
function echo_parts_form($prts,$part_selected=0,$part_current=0,$part_owner=0,$step=0){
	$sep='';
	for($i=0;$i<$step;$i++) $sep.='&nbsp;&nbsp;&nbsp;&nbsp;';
	foreach($prts[$part_owner] AS $pr)if(empty($part_current) || $pr["part_id"]!=$part_current){
		$add='';
		if($part_selected==$pr["part_id"]) $add=' selected';
		echo '<option value="'.$pr["part_id"].'"'.$add.'>'.$sep.$pr["part_name"].'</option>';
		if(!empty($prts[$pr["part_id"]])) echo_parts_form($prts,$part_selected,$part_current,$pr["part_id"],$step+1);
	}
}
if((empty($action) || $action!='edit_part_form') && check_mod($id,'edit')){
echo '<h3 OnClick="showhide(\'part_add\');" style="cursor: pointer;">'.si('add').' Добавить часть</h3><div id="part_add" style="display: none;">';
echo '<form action="mod_main" method="post" name="partadd">
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="action" value="add_part">';

echo '<p>Название<br><input name="part_name" type="text" value="" OnBlur="translate2(this,part_sname);"></p>';
echo '<p>Уникальное спец. название на английском<br><input name="part_sname" type="text" value=""></p>';
echo '<p>Язык<br><select name="part_parse">
	<option value="0">Язык шаблонов</option>';
	
global $user;
if($user->super) echo '<option value="1">Язык PHP</option>';
echo '</select></p>';
echo '<p>Тип<br><select name="part_type" OnChange="
	var obj1=document.getElementById(\'type1\');
	var obj2=document.getElementById(\'type2\');
	var obj1b=document.getElementById(\'type1b\');
	var obj1c=document.getElementById(\'type1c\');
	var obj5=document.getElementById(\'type5\');
	obj5.style.display=\'none\';
	obj1.style.display=\'none\';obj2.style.display=\'none\';obj1b.style.display=\'\';obj1c.style.display=\'\';
	if(this.selectedIndex==0) obj1.style.display=\'\';
	if(this.selectedIndex==1) {obj1c.style.display=\'none\'; obj1.style.display=\'\';}
	if(this.selectedIndex==2) obj2.style.display=\'\';
	if(this.selectedIndex==3) obj2.style.display=\'\';
	if(this.selectedIndex==4){ obj1b.style.display=\'none\'; obj1.style.display=\'\'; obj5.style.display=\'\';}
	//if(this.selectedIndex==5){ obj1b.style.display=\'none\'; obj1.style.display=\'\';}
	">
                  <option value="0">Базовый</option><option value="1">Виджет</option><option value="3">Обработчик</option><option value="4">Обработчик импорта</option><option value="5">Обработчик строки таблицы</option><!-- <option value="6">Обработчик для нескольких строк таблицы</option> --></select></p>';


	//Иконки
	$a5='none';
	echo '<div id="type5" style="display: '.$a5.'">';
	echo '<input type="hidden" name="part_pic" id="part_pic" value="">';
	echo '<p>';
	echo '<span class="link" OnClick="showhide(\'icondiv\');">Ассоциировать с значком</span>';
	echo ' <img id="part_pic2" src="/files/editor/empty.gif" align="middle">';
	echo '</p>';
	echo '<div id="icondiv" style="display:none;">';
	$ics=scan_dir(DOCUMENT_ROOT.'/files/editor/icons/');
	$i2=0;
	foreach($ics AS $i){
		$i2++;
		echo '<img src="/files/editor/icons/'.filename($i).'" hspace=4 vspace=4 class="link" OnClick="icon_click(\''.filename($i).'\');">';
		if($i2 % 15==0) echo '<br>';
	}
	echo '</div>';
	echo '</div>';

echo '<div id="type1">';
$ati=implode(',',getall3($db,"SELECT table_id,table_module FROM main_table WHERE table_module=$id","table_id"));
$ati2='';
if(!empty($ati)){
	$ati2=implode(',',getall3($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($ati)","sub_table2"));
	if(!empty($ati2)) $ati2=" OR table_id IN ($ati2)";
}
$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id".$ati2." ORDER BY table_name",1,'main_table');
if(!empty($tbl)){
	echo '<div id="base-place-for-table"><p id="part_table_link">Связь с таблицей<br><select name="part_table" id="part_table">
		<option value="0">Нет</option>';
	if(!empty($tbl)) foreach($tbl AS $tb){
		echo '<option value="'.$tb["table_id"].'">';
		if($tb["table_module"]!=$id) echo ' - ';
		echo $tb["table_name"].'</option>';
	}
	echo '</select></p></div>';
} else echo '<input type="hidden" name="part_table" value="0">';

echo '<div id="type1b">';
echo '<p>Видимость<br><select name="part_access"><option value="0">Внутри модуля</option><option value="1">Глобальный</option></select></p>';

echo '<div id="type1c">';

/*echo '<p><input type="checkbox" class="button" name="part_ignore" OnClick="this.checked?show(\'type1c2\'):hide(\'type1c2\');"> Использовать для разбора URL</p>';
echo '<div id="type1c2" style="display: none;" class="subdiv">';
echo '<p><input type="checkbox" class="button" name="part_iowner" OnClick="var x=$(\'#part_sowner\'); if(this.checked) x.hide(); else x.show();"> Игнорировать родителя-строку при разборе</p>';
echo '<p id="part_sowner"><input type="checkbox" class="button" name="part_sowner"> Включать в разбор строки из подчинённых таблиц'.se('help','','',' OnClick="showhide(\'part_help2\')" style="cursor: pointer;"',0,5,0).'<div id="part_help2" style="display: none;">'.$part_help2.'</div></p>';
echo '<p id="part_404"><input type="checkbox" class="button" name="part_404"> Запускать в случае, если под URL не подошла ни одна часть</p>';
echo '<p id="part_url" style="padding-right: 20px;">Вызывается по URL:<br><nobr><input name="part_url" type="text" value="">'.se('help','','',' OnClick="showhide(\'part_help\')" style="cursor: pointer;"',0,5,0).'</nobr><div id="part_help" style="display: none;">'.$part_help.'</div></p>';
echo '<p id="part_ifdetect">Условие для вызова:<br><input name="part_ifdetect" type="text" value=""></p>';
echo '</div>';
*/

echo '<p>Участвует в разборе URL:<br><select name="part_ignore" OnChange="url_select(this.selectedIndex);">
	<option value="0">Не участвует</option>
	<option value="1">По точному совпадению</option>
	<option value="2">По совпадению с URL объекта</option>
	<option value="3">По условию для запуска</option>
	<option value="4">Как главная часть (вызов с пустого URL)</option>
	<option value="5">Запускается в случае, если не подошли остальные части</option>
</select></p>';

echo '<div id="type1c2" style="display: none;" class="subdiv">';
echo '<div id="place-for-table"></div>';
echo '<p id="part_iowner"><input type="checkbox" class="button" id="part_iowner_check" name="part_iowner" OnClick="var x=$(\'#part_sowner\'); if(this.checked) x.hide(); else x.show();"> Игнорировать родителя-строку при разборе</p>';
echo '<p id="part_sowner"><input type="checkbox" class="button" name="part_sowner"> Включать в разбор строки из подчинённых таблиц'.se('help','','',' OnClick="showhide(\'part_help2\')" style="cursor: pointer;"',0,5,0).'<div id="part_help2" style="display: none;">'.$part_help2.'</div></p>';
echo '<p id="part_url" style="padding-right: 20px;">Вызывается по URL:<br><nobr><input name="part_url" id="save_url" type="text" value="">'.se('help','','',' OnClick="showhide(\'part_help\')" style="cursor: pointer;"',0,5,0).'</nobr><div id="part_help" style="display: none;">'.$part_help.'</div></p>';
echo '<p id="part_ifdetect">Условие для вызова:<br><input name="part_ifdetect" type="text" value=""></p>';
echo '</div>';

echo '</div></div>';
echo '</div>';

$prt=getall5($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type=0 ORDER BY part_type, part_name","part_owner");
if(!empty($prt)){
	echo '<p>Владелец (если часть используется для URL разбора, то он будет продолжаться относительно владельца):<br><select name="part_owner">
		<option value="0" selected>Нет</option>';
	echo_parts_form($prt);
	echo '</select>';
} else echo '<input type="hidden" name="part_owner" value="0">';

echo '<div id="type2" style="display: none;">';

	$add1='';$add2='';
	echo '<p>Связь с разделом (для запуска обработчика)<br><select name="part_ex">
		<option value="-1"'.$add1.'>Нет связи с разделами</option>
		<option value="0"'.$add2.'>При выполнении перебирать все разделы</option>';
	$exs=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id ORDER BY ex_name");
	if(!empty($exs)) foreach($exs AS $ex) echo '<option value="'.$ex["ex_id"].'">'.$ex["ex_name"].'</option>';
	echo '</select></p>';

	echo '<p>Опорный элемент (E5.cow)<br>';// / при прямом вызове из админки CEX будет равен текущему разделу МОДУЛЯ а не части)<br>';

	echo '<select name="part_cur"><option value="0">Нет</option>';
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	if(!empty($tbl)) foreach($tbl AS $tb){
		if(!empty($GLOBALS["cex".$id])) $ex_ex2=$GLOBALS["cex".$id]; else $ex_ex2=0;
		//if($ex_ex2!=0){			
			/*getrow($db,"SELECT * FROM ex_group WHERE ex_table=".$tb["table_id"]." AND ex_ex2=$ex_ex2",1,"ex_group");
			$ex_ex1=$db->Record["ex_ex1"];		
			$rows=get_vars(get_sub(0,$tb["table_id"],1,1,0,0,1,$ex_ex1,$ex_ex2,$tb["table_id"],0,$tb["table_id"]));			
			$res=options($rows,'   ',0,1,0,0,0,'&nbsp;&nbsp;');
			$res=str_replace(' value="',' value="'.$ex_ex2.':'.$tb["table_id"].':',$res);*/
		//}
		echo '<option value="'.$ex_ex2.':'.$tb["table_id"].':0" style="background-color: #EEEEEE;">Таблица «'.$tb["table_name"].'»</option>'/*.$res*/;
		if($ex_ex2!=0) echo get_simple_options($tb['table_id'],$ex_ex2,0);
	}
	echo '</select></p>';
	
	//echo '<p>Связь с локальным файлом:<br><input name="part_file" type="text" value=""></p>';
	echo '<input type="hidden" name="part_file" value="">';
	//echo '</p>';
echo '</div>';


echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_part\');">'.si('point').'Дополнительные параметры</h2>';
echo '<div id="addition_part" style="display: none;">';

echo '<p>Условие для запуска:<br><input name="part_ifcase" type="text" value=""></p>';

echo '<p>Связанная папка <i>(начинать с /files, должна начинаться и заканчиваться слешем, если нужно указать несколько папок - указывайте через запятую)</i>:<br><input name="part_folder" type="text" value=""></p>';

//echo '<p><input type="checkbox" class="button" name="part_auth"> Всегда запускать с правами текущего пользователя (если установлено, то только текущий пользователь и СуперПользователь смогут редактировать эту часть)</p>';
	echo '<p>Запускать с следующими правами доступа:<br>';
	echo '<select name="part_auth"><option value="-1">Текущие права пользователя</option>';
	if($user->super) echo '<option value="0">С правами суперпользователя</option>';
	$grp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$id ORDER BY group_name");
	if(!empty($grp)) foreach($grp AS $gp){
		echo '<option value="'.$gp["auth_id"].'">'.$gp["group_name"].'</option>';
	}
	echo '</select></p>';
	
echo '<p>Переопределение текущего объекта (следующий неразобранный URL - case_url, после него - lefturl)<br><input name="part_ifrow" type="text" value=""></p>';

echo '<p>Сколько дополнительных сегментов отрезать от URL:<br><input name="part_skipurl" type="text" value="0"></p>';

echo '<p><input type="checkbox" class="button" name="part_unsafe"> Не проверять входящие GET переменные (для AJAX вызовов), а также разрешить вызов через ?ajax=module.ex.part</p>';

echo '</div>';

echo '<input class="button" type="submit" value="Добавить">';
echo '</form></div>';
}

//=======================
//  Форма редактирования части
//=======================
if(!empty($_GET["id2"])) $id2=$_GET["id2"];
if(!empty($action) && $action=='edit_part_form' && !empty($id2) && check_mod($id,'edit') && check_prt($id2,'edit')){
	echo '<a name="edit_part"></a>';
	echo '<h2>'.si('edit2').'Изменить часть</h2>';
	echo '<form action="mod_main" method="post">
		<input type="hidden" name="action" value="edit_part">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM main_part WHERE part_id=$id2 AND part_module=$id",1,'main_part');
	foreach($db->Record AS $var=>$value)$$var=$value;

	echo '<p>Название<br><input name="part_name" type="text" value="'.$part_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="part_sname" type="text" value="'.$part_sname.'"></p>';

	$add1='';$add2='';$add3='';
	if($part_parse==0) $add1=' selected';
	if($part_parse==1) $add2=' selected';
	echo '<p>Язык<br><select name="part_parse">
		<option value="0"'.$add1.'>Язык шаблонов</option>';
	global $user;
	if($user->super)echo '<option value="1"'.$add2.'>Язык PHP</option>';
	echo '</select></p>';

	$add1='';$add2='';$add3='';$add4='';$add5='';$add6='';if($part_type==0) $add1=' selected'; if($part_type==1) $add2=' selected'; if($part_type==3) $add3=' selected';if($part_type==4) $add4=' selected';if($part_type==5) $add5=' selected';if($part_type==6) $add6=' selected';
	echo '<p>Тип<br><select name="part_type" OnChange="
	var obj1=document.getElementById(\'type1\');
	var obj2=document.getElementById(\'type2\');
	var obj5=document.getElementById(\'type5\');
	var obj1b=document.getElementById(\'type1b\');
	var obj1c=document.getElementById(\'type1c\');
	obj5.style.display=\'none\';
	obj1.style.display=\'none\';obj2.style.display=\'none\';obj1b.style.display=\'\';obj1c.style.display=\'\';
	if(this.selectedIndex==0) obj1.style.display=\'\';
	if(this.selectedIndex==1){ obj1c.style.display=\'none\'; obj1.style.display=\'\';}
	if(this.selectedIndex==2) obj2.style.display=\'\';
	if(this.selectedIndex==3) obj2.style.display=\'\';
	if(this.selectedIndex==4){ obj1b.style.display=\'none\'; obj1.style.display=\'\'; obj5.style.display=\'\';}
	">
	<option value="0"'.$add1.'>Базовый</option><option value="1"'.$add2.'>Виджет</option><option value="3"'.$add3.'>Обработчик</option><option value="4"'.$add4.'>Обработчик импорта</option><option value="5"'.$add5.'>Обработчик строки таблицы</option><!-- <option value="6"'.$add6.'>Обработчик для нескольких строк таблицы</option> --> </select></p>';

	//Иконки
	$a5='';if($part_type==5) $a5=''; else $a5='none';
	echo '<div id="type5" style="display: '.$a5.'">';
	echo '<input type="hidden" name="part_pic" id="part_pic" value="'.$part_pic.'">';
	echo '<p>';
	echo '<span class="link" OnClick="showhide(\'icondiv\');">Ассоциировать с значком</span>';
	if($part_pic!='') echo ' <img src="/files/editor/icons/'.$part_pic.'" id="part_pic2" align="middle">'; else echo ' <img id="part_pic2" src="/files/editor/empty.gif" align="middle">';
	echo '</p>';
	echo '<div id="icondiv" style="display:none;">';
	$ics=scan_dir(DOCUMENT_ROOT.'/files/editor/icons/');
	$i2=0;
	foreach($ics AS $i){
		$i2++;
		echo '<img src="/files/editor/icons/'.filename($i).'" hspace=4 vspace=4 class="link" OnClick="icon_click(\''.filename($i).'\');">';
		if($i2 % 15==0) echo '<br>';
	}
	echo '</div>';
	echo '</div>';
	
	// Определение типа URL-разбора
	if($part_ignore){
		$svalue=0;
	} else {
		if(!$part_table && $part_url && !$part_ifdetect) $svalue=1;
		if($part_table) $svalue=2;
		if($part_ifdetect && !$part_table) $svalue=3;
		if(!$part_url && !$part_table && !$part_ifdetect && !$part_404) $svalue=4;
		if($part_404) $svalue=5;
	}

	//Полный / Виджет
	$a1='';if($part_type>1 && $part_type!=5/* && $part_type!=6*/) $a1='none';
	$a3='';if($part_type==5/* || $part_type==6*/) $a3='none';
	$a1b=''; if($part_type==1) $a3='none';// $a1b='none';
	echo '<div id="type1" style="display:'.$a1.'">';
	$ati=implode(',',getall3($db,"SELECT table_id,table_module FROM main_table WHERE table_module=$id","table_id"));
	$ati2='';
	if(!empty($ati)){
		$ati2=implode(',',getall3($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($ati)","sub_table2"));
		if(!empty($ati2)) $ati2=" OR table_id IN ($ati2)";
	}
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id".$ati2." ORDER BY table_name",1,'main_table');
	$echo_tbl='';
	if(!empty($tbl)){
		$echo_tbl.=/*'<div id="base-place-for-table">*/'<p id="part_table_link">Связь с таблицей<br><select name="part_table" id="part_table">
			<option value="0">Нет</option>';
		foreach($tbl AS $tb){
			$add='';
			if($part_table==$tb["table_id"]) $add=' selected';
			$echo_tbl.='<option value="'.$tb["table_id"].'"'.$add.'>';
			if($tb["table_module"]!=$id) $echo_tbl.=' - ';
			$echo_tbl.=$tb["table_name"].'</option>';
		}
		$echo_tbl.='</select></p>';//</div>';
	} else echo '<input type="hidden" name="part_table" value="0">';
	// Непосредственный вывод списка таблиц
	echo '<div id="base-place-for-table">';
	if($svalue!=2) echo $echo_tbl;
	echo '</div>';
	
	echo '<div id="type1b" style="display:'.$a1b.'">';
	$add1='';$add2='';if($part_access==0) $add1=' selected'; else $add2=' selected';
	echo '<p>Видимость<br><select name="part_access"><option value="0"'.$add1.'>Внутри модуля</option><option value="1"'.$add2.'>Глобальный</option></select></p>';
	echo '<div id="type1c" style="display:'.$a3.'">';

	//$add=''; $add2=' style="display: none;"'; if(empty($part_ignore)){$add=' checked'; $add2='';}
	//echo '<p><input type="checkbox" class="button" name="part_ignore"  OnClick="this.checked?show(\'type1c2\'):hide(\'type1c2\');"'.$add.'> Использовать для разбора URL</p>';		
	
	echo '<p>Участвует в разборе URL:<br><select name="part_ignore" OnChange="url_select(this.selectedIndex);">
		<option value="0"'.($svalue==0?' selected':'').'>Не участвует</option>
		<option value="1"'.($svalue==1?' selected':'').'>По точному совпадению</option>
		<option value="2"'.($svalue==2?' selected':'').'>По совпадению с URL объекта</option>
		<option value="3"'.($svalue==3?' selected':'').'>По условию для запуска</option>
		<option value="4"'.($svalue==4?' selected':'').'>Как главная часть (вызов с пустого URL)</option>
		<option value="5"'.($svalue==5?' selected':'').'>Запускается в случае, если не подошли остальные части</option>
	</select></p>';
	
	$add='';
	if($svalue==0 || $svalue==4 || $svalue==5) $add=' style="display: none;"';
	
	echo '<div id="type1c2" class="subdiv"'.$add.'>';	
	
	echo '<div id="place-for-table">';
	if($svalue==2) echo $echo_tbl;
	echo '</div>';
	$add=''; if(!empty($part_iowner))$add=' checked';
	echo '<p id="part_iowner"'.($svalue!=2?' style="display:none;"':'').'><input type="checkbox" class="button" id="part_iowner_check" name="part_iowner" OnClick="var x=$(\'#part_sowner\'); if(this.checked) x.hide(); else x.show();"'.$add.'> Игнорировать родителя-строки при разборе</p>';
	$add=''; if(!empty($part_sowner))$add=' checked';
	echo '<p id="part_sowner"'.((!empty($part_iowner) || $svalue!=2)?' style="display:none;"':'').'><input type="checkbox" class="button" name="part_sowner"'.$add.'> Включать в разбор строки из подчинённых таблиц'.se('help','','',' OnClick="showhide(\'part_help2\')" style="cursor: pointer;"',0,5,0).'<div id="part_help2" style="display: none;">'.$part_help2.'</div></p>';
	//$add=''; if(!empty($part_404))$add=' checked';
	//echo '<p id="part_404"><input type="checkbox" class="button" name="part_404"'.$add.'> Запускать в случае, если под URL не подошла ни одна часть</p>';
	echo '<p style="padding-right: 20px;" id="part_url"'.(($svalue!=1 && $svalue!=2 && $svalue!=3)?' style="display:none;"':'').'>Вызывается по URL:<br><nobr><input name="part_url" id="save_url" type="text" value="'.$part_url.'">'.se('help','','',' OnClick="showhide(\'part_help\')" style="cursor: pointer;"',0,5,0).'</nobr><div id="part_help" style="display: none;">'.$part_help.'</div></p>';
	
	echo '<p id="part_ifdetect"'.($svalue!=3?' style="display:none;"':'').'>Условие для вызова:<br><input name="part_ifdetect" type="text" value="'.htmlspecialchars($part_ifdetect).'"></p>';

	//echo '<p>Условие для вызова<br><input name="part_ifcase" type="text" value="'.$part_ifcase.'"></p>';
		
	//echo '<p><input type="checkbox" class="button" name="part_ignore"'.$add.'> Игнорировать при разборе URL</p>';
	//владелец был тут
	echo '</div>';
	echo '</div></div>';
	echo '</div>';
	
	//а теперь здесь
	//$prt=getall($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type=0 ORDER BY part_name",1,'main_part');/* AND part_ignore=0  - потому что некоторые части с part_ignore=1 могут быть назначены на страницы, а у них тоже могут быть потомки*/
	$prt=getall5($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type=0 ORDER BY part_type, part_name","part_owner");
	if(!empty($prt)){
		echo '<p>Владелец (если часть используется для URL разбора, то он будет продолжаться относительно владельца):<br><select name="part_owner">
			<option value="0">Нет</option>';
		
		echo_parts_form($prt,$part_owner,$part_id);	
		
		echo '</select>';
	} else echo '<input type="hidden" name="part_owner" value="0">';

	//Обработчик
	$a2='';if($part_type<3 || $part_type==5/* || $part_type==6*/) $a2='none';
	echo '<div id="type2" style="display: '.$a2.'">';
	$add1='';$add2='';
	if($part_ex==-1) $add1=' selected';
	if($part_ex==0) $add2=' selected';
	echo '<p>Связь с разделом (для запуска обработчика)<br><select name="part_ex">
		<option value="-1"'.$add1.'>Нет связи с разделами</option>
		<option value="0"'.$add2.'>При выполнении перебирать все разделы</option>';
	$exs=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id ORDER BY ex_name");
	if(!empty($exs)) foreach($exs AS $ex){
		$add='';
		if($ex["ex_id"]==$part_ex) $add=' selected';
		echo '<option value="'.$ex["ex_id"].'"'.$add.'>'.$ex["ex_name"].'</option>';
	}
	echo '</select></p>';
	echo '<p>Опорный элемент (E5.cow)<br>';// / при прямом вызове из админки CEX будет равен текущему разделу МОДУЛЯ а не части))<br>';
	
	echo '<select name="part_cur"><option value="0">Нет</option>';
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	$vstr=$part_cur;
	if(!empty($tbl)) foreach($tbl AS $tb){
		if(empty($GLOBALS["cex".$id])) $ex_ex2=0;
		else $ex_ex2=$GLOBALS["cex".$id];
		
		/*
		$rows=get_vars(get_sub(0,$tb["table_id"],1,1,0,0,1,$ex_ex1,$ex_ex2,$tb["table_id"],0,$tb["table_id"]));
		$res=options($rows,'   ',0,1,0,0,0,'&nbsp;&nbsp;');
		$res=str_replace(' value="',' value="'.$ex_ex2.':'.$tb["table_id"].':',$res);
		$res=str_replace(' value="'.$vstr.'"',' value="'.$vstr.'" selected',$res);
		*/
		
		if($vstr==$ex_ex2.':'.$tb["table_id"].':0') $add=' selected'; else $add='';
		echo '<option value="'.$ex_ex2.':'.$tb["table_id"].':0"'.$add.' style="background-color: #EEEEEE;">Таблица «'.$tb["table_name"].'»</option>'.$res;
		
		echo get_simple_options($tb['table_id'],$ex_ex2,$vstr);			
		
	}
	echo '</select></p>';
	
	//echo '<p>Связь с локальным файлом:<br><input name="part_file" type="text" value="'.$part_file.'"></p>';
	echo '<input type="hidden" name="part_file" value="'.$part_file.'">';
	echo '</div>';
	
	echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_part\');">'.si('point').'Дополнительные параметры</h2>';
	if(empty($part_ifcase) && empty($part_folder) && empty($part_unsafe) && empty($part_ifrow) && $part_auth==-1 && empty($part_skipurl)) echo '<div id="addition_part" style="display: none;">';
	else echo '<div id="addition_part">';
	
	echo '<p>Условие для запуска:<br><input name="part_ifcase" type="text" value="'.htmlspecialchars($part_ifcase).'"></p>';
	echo '<p>Связанная папка <i>(начинать с /files, должна начинаться и заканчиваться слешем, если нужно указать несколько папок - указывайте через запятую)</i>:<br><input name="part_folder" type="text" value="'.htmlspecialchars($part_folder).'"></p>';
	
	//if($part_auth!=-1) $add='checked'; else $add='';
	//echo '<p><input type="checkbox" class="button" name="part_auth" '.$add.'> Всегда запускать с правами текущего пользователя (если установлено, то только текущий пользователь и СуперПользователь смогут редактировать эту часть)</p>';
	
	
	echo '<p>Запускать с следующими правами доступа:<br>';
	echo '<select name="part_auth"><option value="-1">Текущие права пользователя</option>';
	if($part_auth==0 || $user->super) echo '<option value="0"'.($part_auth==0?' selected':'').'>С правами суперпользователя</option>';
	$grp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$id ORDER BY group_name");
	if(!empty($grp)) foreach($grp AS $gp){
		if($part_auth==$gp["auth_id"]) $add=' selected'; else $add='';
		echo '<option value="'.$gp["auth_id"].'"'.$add.'>'.$gp["group_name"].'</option>';
	}
	echo '</select></p>';
	
	echo '<p>Переопределение текущего объекта (следующий неразобранный URL - case_url, после него - lefturl)<br><input name="part_ifrow" type="text" value="'.htmlspecialchars($part_ifrow).'"></p>';
	
	echo '<p>Сколько дополнительных сегментов отрезать от URL:<br><input name="part_skipurl" type="text" value="'.htmlspecialchars($part_skipurl).'"></p>';
	
	if($part_unsafe!=0) $add='checked'; else $add='';
	echo '<p><input type="checkbox" class="button" name="part_unsafe" '.$add.'> Не проверять входящие GET переменные (для AJAX вызовов), а также разрешить вызов через ?ajax=module.ex.part</p>';
	
	if(/*strlen($part_body)>100 && */!strstr($part_body,'shell') && $user->super && $part_parse==0 && ($part_type==0 || $part_type==1)){
		echo '<div align="right"><a href="mod_main?id='.$id.'&amp;id2='.$part_id.'&amp;action=create_template" onclick="return(confirm(\'Вы уверены?\'))">Шаблонизировать часть</a></div>';
	}
	
	echo '</div>';

	echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_main?id='.$id.'">вернуться назад</a>';
	echo '</form>';
}

//=======================
//  Форма настройки тайминга части
//=======================
if(!empty($action) && $action=='timer_form' && !empty($id2) && check_mod($id,'edit')){
	echo '<a name="timer_form"></a><h2>'.si('timer').'Настройка времени запуска</h2>';
		
	echo '<form action="mod_main" method="post">
		<input type="hidden" name="action" value="edit_timer">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM main_part WHERE part_id=$id2 AND part_module=$id",1,'main_part');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<p>Имя части: <b>'.$part_name.'</b> ('.$part_sname.')</p>';

	echo '<div>Время на сервере: '.date('Y-m-d H:i:s').'</div>';	
	if($timer_last!='0000-00-00 00:00:00') echo '<div>Время последнего запуска: '.$timer_last.' (<a href="mod_main?id='.$id.'&amp;id2='.$part_id.'&amp;action=cleartimer">очистить</a>)</div>';

	$add1='';$add2='';$add3='';$add4='';$add5='';
	if($timer_type==0) $add1=' selected';
	if($timer_type==1) $add2=' selected';
	if($timer_type==2) $add3=' selected';
	if($timer_type==3) $add4=' selected';
	echo '<p>Тип<br><select name="timer_type">
		<option value="0"'.$add1.'>Нет автозапуска</option>
		<option value="1"'.$add2.'>Запускается 1 раз во время определённое полями Дата и Время</option>
		<option value="2"'.$add3.'>Запускается каждые X минут</option>
		<option value="3"'.$add4.'>Запускаются каждый день во время, указанное в поле Время</option>
		</select>';
	echo '<p>поле Дата (Год-месяц-день):<br><input name="timer_date" type="text" value="'.$timer_date.'"></p>';
	echo '<p>поле Время (Часы:минуты:секунды):<br><input name="timer_time" type="text" value="'.$timer_time.'"></p>';
	if(empty($timer_x)) $timer_x='0';
	echo '<p>поле X:<br><input name="timer_x" type="text" value="'.$timer_x.'"></p>';
	//echo '<p>поле Y:<br><input name="timer_y" type="text" value="'.$timer_y.'"></p>';
	echo '<input name="timer_y" type="hidden" value="0">';

	echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_main?id='.$id.'">вернуться назад</a>';
	echo '</form>';
}

?>