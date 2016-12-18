<?php

$ruser=getall($db,"SELECT * FROM row_user WHERE ru_row=$id3",1,"row_user");
getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=$id2",1,"ex_group");
$table=$db->Record["ex_table"];
$mex=$db->Record["ex_ex2"];
//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3",1,"main_row");
seek_rlink($id3);
global $rlink;
$usr=$rlink[$id3]->user;//$db->Record["row_user"];
$can_edit=check_row($id3,$table,$mex,'edit',$usr,$rlink[$id3]->users,$id);
$can_view=check_row($id3,$table,$mex,'view',$usr,$rlink[$id3]->users,$id);
$name=get_basename($id3,$table);
if($use_titles) echo '<h1>Владельцы строки «'.get_basename($id3).'»</h1>';
echo '<div style="padding-bottom: 5px;">'.se('back','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7).'<a href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7.'">Назад к таблице</a></div>';

if(!empty($action) && $action=='base' && !empty($id8)){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$id8,1,"main_auth");
	if(check_user(-$id8,'view',$db->Record["auth_owner"]) && $can_edit){
		$old_base=getrowval("SELECT ro_user FROM row_owner WHERE row_id=$id3","ro_user");
		$db->query("UPDATE row_user SET ru_user=$old_base WHERE ru_row=$id3 AND ru_user=$id8",3,"row_user");
		$db->query("UPDATE row_owner SET ro_user=$id8 WHERE row_id=$id3",3,"row_owner");
		$db->query("UPDATE main_row SET row_user=$id8 WHERE row_id=$id3",3,"row_owner");
	}
	$ruser=getall($db,"SELECT * FROM row_user WHERE ru_row=$id3",1,"row_user");
	if(count($ruser)==0){
		$db->query("UPDATE row_owner SET ro_users=0 WHERE row_id=".$id3,3,"ro_owner");
	}
	$usr=$id8;
}

if($can_view){
	if($usr==-1) $usrname='Суперпользователь';
	if($usr==0) $usrname='Гость';
	if($usr>0){
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$usr,1,"main_auth");
		$usrname=$db->Record["user_login"].' ('.$db->Record["user_name"].')';
		if(!check_user(-$usr,'view',$db->Record["auth_owner"])) $usrname='';;
	}
	//if(!empty($usrname)){
	//	echo '<div><b>Основной владелец</b>: '.$usrname;
	//}
}

if(!empty($action) && $action=='del' && !empty($id8)){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$id8,1,"main_auth");
	if(check_user(-$id8,'view',$db->Record["auth_owner"]) && $can_edit) $db->query("DELETE FROM row_user WHERE ru_row=".$id3." AND ru_user=".$id8,1,"row_user");
	$ruser=getall($db,"SELECT * FROM row_user WHERE ru_row=$id3",1,"row_user");
	if(count($ruser)==0){
		$db->query("UPDATE row_owner SET ro_users=0 WHERE row_id=".$id3,3,"ro_owner");
	}
}

if(!empty($action) && $action=='add' && !empty($id8)){
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$id8,1,"main_auth");
	if(check_user(-$id8,'view',$db->Record["auth_owner"]) && $can_edit){
		$db->query("INSERT INTO row_user (ru_row, ru_user) VALUES (".$id3.",".$id8.")",3,"row_user");	
		$db->query("UPDATE row_owner SET ro_users=1 WHERE row_id=".$id3,3,"ro_owner");
	}
	$ruser=getall($db,"SELECT * FROM row_user WHERE ru_row=$id3",1,"row_user");
}

if((!empty($ruser) || !empty($usrname)) && $can_view){
	echo '<br><table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th width="50%">Владелец</th>';
	echo '<th>Действия</th>';
	echo '</tr>';
	if(!empty($usrname)){
		echo '<tr>';
		echo '<td>'.$usrname.'</td>';
		echo '<td><b>Основной пользователь</b></td>';
		echo '</tr>';
	}
	foreach($ruser AS $usr){	
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$usr["ru_user"],1,"main_auth");
		$usrname=$db->Record["user_login"].' ('.$db->Record["user_name"].')';
		if(check_user(-$usr["ru_user"],'view',$db->Record["auth_owner"])){
			echo '<tr>';
			echo '<td>'.$usrname.'</td>';
			echo '<td>';
			if($can_edit){
				echo ' <a href="row_user?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7.'&amp;id3='.$id3.'&amp;id8='.$usr["ru_user"].'&amp;action=base" onclick="return(confirm(\'Вы уверены?\'))">Сделать основным</a>';
				echo ' <a href="row_user?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7.'&amp;id3='.$id3.'&amp;id8='.$usr["ru_user"].'&amp;action=del" onclick="return(confirm(\'Вы уверены?\'))">Удалить</a>';
			}
			echo '</td></tr>';
		}
	}
	echo '</table>';
}

$s=select_users();
if($can_edit && !empty($s)){

	echo '<br><br><h3 OnClick="showhide(\'group_add\');" style="cursor: pointer;">'.si('useradd').'Добавить владельца</h3><div id="group_add" style="display: none;">';
	echo '<form action="row_user" method="post">
		<input type="hidden" name="action" value="add">
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id6" value="'.$id6.'">
		<input type="hidden" name="id7" value="'.$id7.'">
		<input type="hidden" name="id3" value="'.$id3.'">
		';
		echo '<p>Выберите пользователя: <br><select name="id8">';
		echo $s;
		echo '</select></p>';
			
	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form></div>';

}

?>