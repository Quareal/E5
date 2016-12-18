<?php

global $id,$id2,$type;
if(empty($_GET["id2"]) && empty($_POST["id2"])) {include('modules.php'); exit;}
if(isset($type) && $user->super==0){include('main.php'); exit;}
if(isset($id2) && isset($id) && !check_mod($id,'edit')){include('main.php'); exit;}
if(!check_prt($id2,'edit')) {include('main.php'); exit;}
if(empty($_GET["id"]) && empty($_GET["id2"]) && empty($_POST["id2"]) && empty($_POST["id"])) {include('modules.php'); exit;}
if(empty($type) && empty($id) && empty($id2)) { include('modules.php'); exit;}
if(!isset($type)){
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,'main_module');
	if(empty($db->Record)){ include('modules.php'); exit;}
	foreach($db->Record AS $var=>$value) $$var=$value;
}
getrow($db,"SELECT * FROM main_part WHERE part_id=$id2",1,'main_part');
foreach($db->Record AS $var=>$value) $$var=$value;

// Чтобы обезопасить от переменных объявленных в foreach
//foreach($_POST AS $var=>$value) $$var=$value;
//foreach($_GET AS $var=>$value) $$var=$value;

if(!isset($type)){
	if($use_titles){
		echo '<h1>Часть «'.$part_name.'»</h1>';
		echo '<h2 align="center">Модуль «'.$module_name.'»</h2>';
		echo '<div align="right"><a href="mod_main?id='.$id.'">Назад к модулю «'.$module_name.'»</a></div>';
		echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
		echo '<br><div align="right"><a href="parts_param?id='.$id.'&amp;id2='.$id2.'">Переменные</a></div>';
	}
} else {
	if($type==0) $pn2='функций';
	if($type==1) $pn2='отображений';
	if($type==2) $pn2='компонентов';
	if($type==3) $pn2='форм';
	if($type==0) $pn='Функция';
	if($type==1) $pn='Отображение';
	if($type==2) $pn='Компонент';
	if($type==3) $pn='Форма';
	if($type==0) $pn3='функцию';
	if($type==1) $pn3='отображение';
	if($type==2) $pn3='компонент';
	if($type==3) $pn3='форму';
	if($use_titles){
		echo '<h1>'.$pn.' «'.$part_name.'»</h1>';
		echo '<div align="right"><a href="parts?type='.$type.'">Назад к списку '.$pn2.'</a></div>';
		echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
		echo '<br><div align="right"><a href="parts_param?type='.$type.'&amp;id2='.$id2.'">Переменные</a></div>';
	}
}

//=======================
//  Редактирование части
//=======================
if(!empty($action) && $action=='edit_part'){
	if(!isset($type)){
		update_module_state($id);
		update_part_links($id,'part',getrowval("SELECT part_id, part_uin FROM main_part WHERE part_id=$id2","part_uin"),$part_body);
	} else {
		update_part_links($id2,'part',getrowval("SELECT part_id, part_uin FROM main_part WHERE part_id=$id2","part_uin"),$part_body,'part');
	}
	del_cache('part',$id2);
	getrow($db,"SELECT * FROM main_part WHERE part_id=$id2",1,"main_part");
	//$part_body=prepend_value($_POST['part_body']);//str_replace("'","''",$_POST['part_body']);
	$part_body=$_POST['part_body'];
	$db->query("UPDATE main_part SET
		part_body='$part_body',
		part_date='".date('Y-m-d H:i:s')."'
		WHERE part_id=$id2",3,'main_part');
}

//==========================
//  Форма редактирования части
//==========================
if(!isset($type)) echo '<h3>Редактировать часть</h3>';
else echo '<h3>Редактировать '.$pn3.'</h3>';

echo '<script>

</script>
';

echo '<form action="mod_part" method="post" name="submitform">';
	if(!isset($type)) echo '<input type="hidden" name="id" value="'.$id.'">';
	else echo '<input type="hidden" name="type" value="'.$type.'">';
	echo '<input type="hidden" name="id2" value="'.$id2.'">
	<input type="hidden" name="action" value="edit_part">';
getrow($db,"SELECT * FROM main_part WHERE part_id=$id2",1,'main_part');
foreach($db->Record AS $var=>$value) $$var=$value;

if($part_parse==0){
	global $use_ace;
	if(!isset($type)) $t=-1;
	else $t=$type;
	ide($part_body,$t,$id,$id2,$part_type,'part_body','part',$use_ace,1);
} else if($part_parse==1){
	show_ace_editor_simple('php','part_body',$part_body,'95%','600px');
} else {
	$data=str_replace('</textarea>','{-{-{/textarea>',$part_body);//защита от кривых кодировок
	$data=htmlspecialchars($data,ENT_QUOTES);
	echo '<textarea name="part_body" id="part_body" style="width: 95%; height: 600px;">'.$data.'</textarea>';
}

if(!isset($type)) echo '<br><br><input class="button" type="submit" value="Сохранить" OnClick="form_submit(); return false;"> или <a href="mod_part?id='.$id.'&amp;id2='.$id2.'">вернуть исходный текст</a>';
else  echo '<br><br><input class="button" type="submit" value="Сохранить" OnClick="form_submit(); return false;"> или <a href="mod_part?type='.$type.'&amp;id2='.$id2.'">вернуть исходный текст</a>';
echo '</form>';

?>