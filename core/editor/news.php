<?php

if(!empty($action) && $action=='all_ok'){
	$n=get_news();
	if($n){
		$ids=Array();
		foreach($n AS $nn)$ids[$nn->id]=$nn->id;
		$ids=implode(',',$ids);
		if(!empty($ids)){
			global $db;
			getrow($db,"UPDATE main_news SET news_read=$user->id WHERE news_id IN ($ids)",3,"main_news");
		}
	}
}
if(!empty($action) && $action=='all_ok2'){
	$n=get_news(0,0,-1,$user->id);
	if($n){
		$ids=Array();
		foreach($n AS $nn)$ids[$nn->id]=$nn->id;
		$ids=implode(',',$ids);
		if(!empty($ids)){
			global $db;
			getrow($db,"UPDATE main_news SET news_read=$user->id WHERE news_id IN ($ids)",3,"main_news");
		}
	}
}
if(!empty($action) && $action=='ok'){
	$n=get_news(0,0,-1,-2,-2,0,$id);
	if($n){
		global $db,$user;
		getrow($db,"UPDATE main_news SET news_read=$user->id WHERE news_id=$id",3,"main_news");
	}
}

$news=get_news();

if(empty($news)) echo 'Новостей нет'; else {
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th width="80">Отметка</th>';
	echo '<th width="150">Дата-время</th>';
	echo '<th width="80">Обзор</th>';	
	echo '<th>Новость</th>';
	echo '<th>Модуль</th>';
	echo '<th>Автор</th>';
	echo '<th width="80">Источник</th>';
	echo '</tr>';
	$ok_user=false;
	foreach($news AS $n){
		echo '<tr>';
		echo '<td align="center"><input type="submit" value="OK" OnClick="document.location.href=\'news?action=ok&id='.$n->id.'\';"></td>';				
		echo '<td align="center">'.get_normal_date($n->date).' '.$n->time.'</td>';
		if($n->group==$user->id) $ok_user=true;
		if($n->url!='') echo '<td align="center"><a href="'.$n->url.'">Обзор</a></td>'; else echo '<td>нет доступа</td>';
		echo '<td><a name="id'.$n->id.'"></a>'.as_html($n->title).'</td>';
		echo '<td>'.$n->module_name.($n->ex_name?' - '.$n->ex_name:'').'</td>';
		echo '<td>'.$n->user.'</td>';
		if($n->from_url!='%admin%') echo '<td align="center"><a href="'.$n->from_url.'">Обзор</a></td>'; else echo '<td align="center">Админка</td>';
		echo '</tr>';
	}
	echo '</table>';
	echo '<div style="padding-top: 10px;"><input type="submit" class="button" value="Все OK" OnClick="document.location.href=\'news?action=all_ok\';">';
	if($ok_user) echo ' <input type="submit" class="button" value="OK адресованные только мне" OnClick="document.location.href=\'news?action=all_ok2\';">';
	echo '</div>';
}

?>