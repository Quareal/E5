<?php

if(empty($_GET["id"]) && empty($_POST["id"])) {include('modules.php'); exit;}
getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,'main_module');

if(empty($db->Record)) {include('modules.php'); exit;}
foreach($db->Record AS $var=>$value) $$var=$value;
if(!check_mod($id,'view')){include('main.php'); exit;}

if(!empty($_COOKIE["cex".$id])){
	$cex=$_COOKIE["cex".$id];
	getrow($db,"SELECT * FROM ex_module WHERE ex_id=$cex AND ex_module=".$id,1,"ex_module");
	if(empty($db->Record)) $cex=-1;
	$GLOBALS["cex".$id]=$cex;
} else $cex=-1;

function reload_exs(){
	global $aex,$db,$id;
	$aex2=getall4($db,"SELECT * FROM ex_module WHERE ex_module=$id","ex_id");
	$aex=Array();
	if(!empty($aex2)) foreach($aex2 AS $ex_id=>$tmp) if(check_ex($ex_id,'view')) $aex[$ex_id]=$tmp;
}

reload_exs();

// Чтобы обезопасить от переменных объявленных в foreach
//foreach($_POST AS $var=>$value) $$var=$value;
//foreach($_GET AS $var=>$value) $$var=$value;

if($use_titles){
	echo '<h1>Модуль «'.$module_name.'»</h1>';
	echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
}

$buff='';

function mprep(&$val){
	$val=str_replace("'","''",$val);
	$val=str_replace("\\","\\\\",$val);
}

// Обработчики
include('mod_main/table_proc.inc');
include('mod_main/ex_proc.inc');
include('mod_main/part_proc.inc');

// Верхняя плашка
global $genter;
echo '<div style="clear: both; height: 25px;">';
echo '<div style="float: left;">';
$eid=-1;
if(!empty($aex)){
	echo si('back2');
	$sel1='<select name="fast_go" class="button" OnChange="document.location=\'mod_main?id='.$id.'&action=set_cex&id2=\'+this.value;">';
	$sel1.='<option value="-1">Выбрать все</option>';
	$seld=Array();
	if(isset($_COOKIE["cex".$id])) $eid=$_COOKIE["cex".$id];
	if(!empty($action) && $action=='set_cex') $eid=$id2;
	foreach($aex AS $ex_id=>$tmp){
		$sel='';
		if($ex_id==$eid) $sel=' selected';
		$tt=$tmp["ex_name"];
		if(strlen5($tt)>60) $tt=substr($tt,0,60).'…';
		$sel1.='<option value="'.$ex_id.'"'.$sel.'>'.$tt.'</option>';
	}
	$sel1.='</select>';
	echo $sel1;
	if(isset($eid) && $eid!=-1 && check_ex($eid,'edit')) echo se('back_config','mod_main?id='.$id.'&id2='.$eid.'&action=edit_ex_form#edit_ex','Изменить раздел');
}
echo '</div>';
echo '<div style="float: right;">';
echo se('back_modules','modules');
if($genter==1 || $user->super) echo se('group','group?fmod='.$id,'Группы доступа');
if(check_mod($id,'edit')) echo se('back_table','mod_col?id='.$id,'Переменные модуля');
echo '</div>';
echo '</div>';

// Круги-таблицы

echo '<div style="clear: both;">';
if(isset($eid) && $eid!=-1){
	$tbls=Array();
	$tmp=getall4($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold!=2 ORDER BY table_bold DESC, table_name","table_id");
	if(!empty($tmp)) foreach($tmp AS $table_id=>$tbl) if(check_tbl($table_id,'view')) $tbls[$table_id]=$tbl;
	$exs=Array();
	$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_module=$id");
	if(!empty($tmp)) foreach($tmp AS $tm) if(check_ex($tm["ex_ex2"],'view')) $exs[$tm["ex_table"]][$tm["ex_ex2"]]=$tm;
	$emj=getrow2("SELECT * FROM ex_module WHERE ex_id=$eid","ex_major");
	if(!empty($tbls)) foreach($tbls AS $tbl)if(!empty($exs[$tbl["table_id"]][$eid])){
		$tid=$exs[$tbl["table_id"]][$eid]["ex_ex1"];
		echo '<div style="float: left; width: 90px; height: 85px; padding: 5px; margin: 5px; margin-right: 20px; margin-bottom: 30px;" align="center">';
		$table_icon=$base_root.'/files/editor/table';
		if($emj==0 && $tbl["table_bold"]==1 || $emj==$tbl["table_id"]) $table_icon.=2;
		$table_icon.='.png';
		if($tbl['table_icon']!='') $table_icon=$base_root.$tbl['table_icon'];
		echo '<a href="'.$GLOBALS["zone_url2"].'/mod_table?id='.$id.'&amp;id2='.$tid.'" class="ablack"><img src="'.$table_icon.'" border="0" style="margin-bottom: 5px;"><br>'.$tbl["table_name"].'</a>';
		echo '</div>';
	}
} else echo '<div style="margin-top: 10px; margin-left: 34px;">Для доступа к таблицам данных, выберите раздел</div>';
echo '</div>';

echo '<div style="clear: both; background-color: #EAEAEA; height: 5px; border-bottom: 1px solid #535353; margin-top: 15px; margin-left: -15px; margin-right: -15px;"></div>';

// Выводим системные сообщения
echo $buff;

echo '<table width="100%" cellpadding="0" cellspacing="0"><tr><td valign="top" width="50%">';
	if(isset($action) && $action=='table_search' && !empty($query)){
		// Поиск по данным
		include('mod_main/table_search.inc');
	} else {
		// Выводим список таблиц и формы
		include('mod_main/table_list.inc');
		echo '<br>';
		// Выводим список экземпляров и формы
		include('mod_main/ex_list.inc');
	}
echo '</td><td width="10">&nbsp;&nbsp;&nbsp;&nbsp;</td><td width="50%" valign="top">';
	// Выводим список частей и формы
	if(isset($action) && $action=='part_search' && !empty($query)){
		include('mod_main/part_search.inc');
	} else include('mod_main/part_list.inc');
echo '</td></tr></table>';



?>