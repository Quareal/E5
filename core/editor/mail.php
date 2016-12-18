<?php

global $user;
if($user->id==0 && !$user->super){include('main.php'); exit;}
$gt='';
if(!empty($user->group)) foreach($user->group AS $g){
	if($gt!='') $gt.=',';
	$gt.=$g;
}
if(!empty($gt)) $gti=" OR mail_to IN (".$gt.")"; else $gti='';

//=============================
//  Удаление сообщений
//=============================
if(isset($del) && check_form_protection_key($_GET['key'],'mail',1)){
	getrow($db,"DELETE FROM main_mail WHERE mail_id=$del AND mail_to=".$user->id." OR (mail_from=".$user->id." AND mail_read=0)",1,",main_mail");
}

//=============================
//  Отправка сообщений
//=============================
if(isset($mail_to) && check_form_protection_key($_POST['key'],'mail',1)){
	$db->query("INSERT INTO main_mail (mail_to, mail_topic, mail_body, mail_date, mail_from, mail_read)
				VALUES ($mail_to, '$mail_topic', '$mail_body', '".date('Y-m-d H:i:s')."', ".$user->id.",0)",3,"main_mail");
}

//=============================
//  Подготовка перед просмотром сообщения
//=============================

global $mtype;
if(isset($set_type)){
	SetCookie("mtype",$set_type,time()+3600*24);
	$mtype=$set_type;
}

if(!empty($view)){
	getrow($db,"SELECT * FROM main_mail WHERE mail_id=$view AND (mail_to=".$user->id." OR mail_from=".$user->id.$gti.")",1,"main_mail");
	if(!empty($db->Record)){
		$m=$db->Record;
		if($mtype==0 && $m["mail_to"]!=$user->id){
				$tmp=explode(',',$m["mail_read"]);
				$b=true;
				foreach($tmp AS $i=>$tm) if($tm==$user->id && strlen($tm)>0) $b=false;
				if($b){
					if($m["mail_read"]=='' OR $m["mail_read"]==0) $t=','.$user->id.','; else $t=$m["mail_read"].$user->id.',';
					$db->query("UPDATE main_mail SET mail_read='".$t."' WHERE mail_id=".$m["mail_id"],3,"main_mail");
				}
		}
		if($mtype==0 && $m["mail_to"]==$user->id){
			$db->query("UPDATE main_mail SET mail_read=1 WHERE mail_id=".$m["mail_id"],3,"main_mail");
		}
	}
}

//=============================
//  Обзор писем
//=============================

echo '<div style="padding-top: 10px; padding-bottom: 10px;">';
$m1=getall($db,"SELECT * FROM main_mail WHERE mail_to=".$user->id.$gti." ORDER BY mail_date DESC");
$ll=-1;
if(!empty($m1)){
	$un=0;
	foreach($m1 AS $m){
		if($m["mail_to"]==$user->id && $m["mail_read"]==0) $un++;
		if($m["mail_to"]!=$user->id && !strstr($m["mail_read"],','.$user->id.',')) $un++;
	}
	if($un!=0) $un='<span style="font-size: 18px; color: #009900;">'.$un.'</span> / '; else $un='';
	if($mtype==0) echo '<b>Входящие</b>'; else echo '<a href="mail?set_type=0">Входящие</a>';
	echo ' ('.$un.count($m1).') &nbsp;&nbsp;&nbsp;';
}
$m2=getall($db,"SELECT * FROM main_mail WHERE mail_from=".$user->id." ORDER BY mail_date DESC");
if(!empty($m2)){
	$un=0;
	foreach($m2 AS $m) if($m["mail_read"]==0) $un++;
	if($un!=0) $un='<span style="font-size: 18px; color: #990000;">'.$un.'</span> / '; else $un='';
	if($mtype==1) echo '<b>Исходящие</b>'; else echo '<a href="mail?set_type=1">Исходящие</a>';
	echo ' ('.$un.count($m2).')';
}
echo '</div>';
if($mtype==0) $mm=$m1; else $mm=$m2;
if(!empty($mm)){
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	if($mtype==0) echo '<th>От кого</th>'; else echo '<th>Кому</th>';
	echo '<th>Заголовок</th>';
	echo '<th>Дата</th>';
	echo '<th>Статус</th>';
	echo '<th width="270">Действия</th>';
	echo '</tr>';
	foreach($mm AS $m){
		echo '<tr>';
		echo '<td>';
		if($mtype==0) $u=$m["mail_from"]; else $u=$m["mail_to"];
		if($ll==$u && 1==2){
			echo '==';
		} else {
			getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$u,1,"main_auth");
			if(empty($db->Record)){ if($u==-1) echo 'SuperUser'; else if($u==0) echo 'Гость'; else echo '-';}
			else {
				if($db->Record["auth_type"]==0) echo $db->Record["user_name"].' ('.$db->Record["user_login"].')';
				else echo 'Группа '.$db->Record["group_name"];
			}
		}
		$ll=$u;
		echo '</td>';
		echo '<td>'.$m["mail_topic"].'</td>';
		$d=explode(' ',$m["mail_date"]);
		$b=get_normal_date($d[0]);
		$b2=explode(':',$d[1]);
		if($d[0]==date('Y-m-d')) $b='Сегодня в';
		echo '<td>'.$b.' '.$b2[0].':'.$b2[1].'</td>';
		echo '<td>';
		if($m["mail_read"]=='0') echo '<b>Не прочитано</b>';
		if($m["mail_read"]=='1') echo 'Прочитано';
		if($m["mail_read"]!='0' && $m["mail_read"]!='1'){
			$tmp=explode(',',$m["mail_read"]);
			$mread=count($tmp)-2;
			if($mtype==1){
				echo $mread.' '.get_str_num($mread,'человек прочитал','человека прочитало','человек прочитало');
			} else {
				$b=true;
				foreach($tmp AS $i=>$tm) if($tm==$user->id && strlen($tm)>0) $b=false;
				if($b) echo '<b>Не прочитано</b>'; else echo 'Прочитано';
			}
		}
		echo '</td>';
		echo '<td>';
		echo '<a href="mail?view='.$m["mail_id"].'">Прочитать</a>';
		if(($m["mail_read"]==0 && $m["mail_from"]==$user->id) || $m["mail_to"]==$user->id) echo ' <a href="mail?del='.$m["mail_id"].'&key='.get_form_protection_key('mail',1,0).'" onclick="return(confirm(\'Вы уверены?\'))">Удалить</a>';
		echo '</td>';
		echo '</tr>';
	}
	echo '</table>';
}

//=============================
//  Обзор письма
//=============================

$top='';
if(!empty($view)){
	getrow($db,"SELECT * FROM main_mail WHERE mail_id=$view AND (mail_to=".$user->id." OR mail_from=".$user->id.$gti.")",1,"main_mail");
	if(!empty($db->Record)){
		$m=$db->Record;
		echo '<h2>Чтение письма</h2>';
		if($mtype==0){
			$to=$m["mail_from"];
			if(strstr($m["mail_topic"],'Re[')){
				$str=explode(']: ',$m["mail_topic"]);
				$str2=explode('[',$str[0]);
				$top='Re['.($str2[1]+1).']: '.$str[1];
			} else if(strstr($m["mail_topic"],'Re:')){
				$top='Re[2]: '.str_replace('Re: ','',$m["mail_topic"]);
			} else $top='Re: '.$m["mail_topic"];
		}

		if($mtype==0) $u=$m["mail_from"]; else $u=$m["mail_to"];
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$u,1,"main_auth");
		if($mtype==0) echo '<p>Отправитель: '; else echo '<p>Получатель: ';
		if(empty($db->Record)){ if($u==-1) echo 'SuperUser'; else if($u==0) echo 'Гость'; else echo '-';}
		else {
			if($db->Record["auth_type"]==0) echo $db->Record["user_name"].' ('.$db->Record["user_login"].')';
			else echo 'Группа '.$db->Record["group_name"];
		}
		echo '</p><p>Тема: '.$m["mail_topic"].'</p><p>Дата: ';
		$d=explode(' ',$m["mail_date"]);
		$b=get_normal_date($d[0]);
		$b2=explode(':',$d[1]);
		if($d[0]==date('Y-m-d')) $b='Сегодня в';
		echo $b.' '.$b2[0].':'.$b2[1];
		echo '</p><p><B>Текст письма</B>:<br>';
		echo $m["mail_body"];
		echo '</p><br><Br>';

	
	}
}

//=============================
//  Форма написания письма
//=============================
if(isset($view) && $mtype==0) echo '<h2 style="cursor: pointer;" OnClick="JavaScript: showhide(\'mail\');">Ответить на письмо</h2>';
else echo '<h2 style="cursor: pointer;" OnClick="JavaScript: showhide(\'mail\');">Написать письмо</h2>';
echo '<form action="mail" method="post" id="mail"';
if(!isset($to) && ((!isset($view)) || $mtype==1)) echo ' style="display: none"';
echo '>';
echo get_form_protection_key('mail',1,1);
echo '<p>Кому:<br><select name="mail_to">';
if($user->id!=-1) echo '<option value="-1">SuperUser</option>';
$tos=getall($db,"SELECT * FROM main_auth WHERE auth_enable=1 ORDER BY auth_type, user_name, group_name");
foreach($tos AS $t)if($t["auth_id"]!=$user->id && (($t["auth_type"]==0 && check_user(-$t["auth_id"],'view',$t["auth_owner"]))||($t["auth_type"]==1 && check_group($t["auth_id"],'view')))){
	$add='';
	if(!empty($to) && $to==$t["auth_id"]) $add=' selected';
	echo '<option value="'.$t["auth_id"].'"'.$add.'>';
	if($t["auth_type"]==0) echo $t["user_name"].' ('.$t["user_login"].')'; else echo 'Группа "'.$t["group_name"].'"';
	echo '</option>';
}
echo '</select></p>';
echo '<p>Заголовок<br><input name="mail_topic" type="text" value="'.$top.'"></p>';
echo '<p>Текст письма<br><textarea name="mail_body" style="width: 500px; height: 400px;"></textarea></p>';
echo '<input class="button" type="submit" value="Добавить">';
echo '</form>';

?>