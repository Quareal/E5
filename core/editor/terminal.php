<?php

global $user; if($user->super==0) {include('main.php'); exit;}

global $lang, $code_sql, $code_php, $code_e5, $code_terminal, $user_e5, $id;

if(empty($lang)) $lang='sql';
$prc=shell_exec('ps axu');
if(!empty($prc)) $prc=explode(chr(10),$prc);
$have_process=true;
if((empty($prc) || count($prc)<=1)){
	$have_process=false;
	if($lang=='process') $lang='sql';
}

if(empty($code)) $code='';
echo '<form action="terminal" method="post"><input type="hidden" name="action" value="do">';
echo get_form_protection_key('terminal',1,1);
$add1=' selected';$add2='';$add3='';$add4='';$add5='';
if($lang=='sql') $add1=' selected';
if($lang=='php') $add2=' selected';
if($lang=='e5') $add3=' selected';
if($lang=='terminal') $add4=' selected';
if($lang=='process') $add5=' selected';
echo '<select name="lang" style="" OnChange="';
echo "var n='code_';
	document.getElementById('code_sql_div').style.display='none';
	document.getElementById('code_php_div').style.display='none';
	document.getElementById('code_e5d').style.display='none';
	document.getElementById('user_e5').style.display='none';
	document.getElementById('code_terminal').style.display='none';
	document.getElementById('code_process').style.display='none';
	document.getElementById('shell_btn').style.display='';
	if(this.selectedIndex==0) n+='sql_div';
	if(this.selectedIndex==1) n+='php_div';
	if(this.selectedIndex==2) n+='e5d';
	if(this.selectedIndex==3) n+='terminal';
	if(this.selectedIndex==4) n+='process';
	document.getElementById(n).style.display='';
	if(this.selectedIndex==2) document.getElementById('user_e5').style.display='';
	if(this.selectedIndex==4) document.getElementById('shell_btn').style.display='none';
	";
echo '">
		<option value="sql"'.$add1.'>SQL</option>
		<option value="php"'.$add2.'>PHP</option>
		<option value="e5"'.$add3.'>E5</option>
		<option value="terminal"'.$add4.'>Terminal / Cmd</option>';
	if($have_process) echo '<option value="process"'.$add5.'>Список процессов</option>';
	echo '</select>';
	
$add1=' display: none;'; $add2=' display: none;'; $add3=' display: none;'; $add4=' display: none;'; $add5=' display: none;';
if(empty($lang)) $add1='';
if($lang=='sql') $add1='';
if($lang=='php') $add2='';
if($lang=='e5') $add3='';
if($lang=='e5') $add5='';
if($lang=='terminal') $add4='';
$code_php=str_replace(Array("\\'",'\\"','\\\\'),Array("'",'"','\\'),$code_php);
$code_sql=str_replace(Array("\\'",'\\"','\\\\'),Array("'",'"','\\'),$code_sql);
$code_e5=str_replace(Array("\\'",'\\"','\\\\'),Array("'",'"','\\'),$code_e5);
$code_terminal=str_replace(Array("\\'",'\\"','\\\\'),Array("'",'"','\\'),$code_terminal);
//echo '<textarea name="code_sql" id="code_sql" style="height: 200px;'.$add1.'">'.$code_sql.'</textarea>';
echo '<div style="'.($lang!='sql'?'display: none;':'').'" id="code_sql_div">';
show_ace_editor_simple('sql','code_sql',$code_sql,'99%','200px');
echo '</div>';

//echo '<textarea name="code_php" id="code_php" style="height: 200px;'.$add2.'">'.$code_php.'</textarea>';
echo '<div style="'.($lang!='php'?'display: none;':'').'" id="code_php_div">';
show_ace_editor_simple('php','code_php',$code_php,'99%','200px');
echo '</div>';

if(!isset($user_e5)) $user_e5=-1;
echo '<select name="user_e5" id="user_e5" style="'.$add5.'">';
echo select_users($user_e5,1,1,1);
echo '</select>';
//echo '<textarea name="code_e5" id="code_e5" style="height: 200px;'.$add3.'">'.$code_e5.'</textarea>';
if(empty($code_e5)) $code_e5='';
echo '<div id="code_e5d" style="'.$add3.'">';
//global $use_ace;
//ide($code_e5,1,0,0,0,'code_e5','terminal',$use_ace,0);
show_ace_editor('html','code_e5',$code_e5,'99%','200px');
echo '</div>';

echo '<textarea name="code_terminal" id="code_terminal" style="height: 200px;'.$add4.'">'.$code_terminal.'</textarea>';
echo '<input type="submit" value="Выполнить" id="shell_btn" class="button" style="'.($lang=='process'?'display: none;':'').'">';
echo '</form>';

echo '<br><br>';

if(!empty($action) && $action=='do' && !empty($lang) && check_form_protection_key($_POST['key'],'terminal',1)){
	if($lang=='sql' && !empty($code_sql)){
		global $db;
		$sql=str_replace('\\'."'","'",$code_sql);
		$db->query($sql);
		@$xn=mysql_affected_rows($db->Link_ID);
		if($xn>0) $xn='<div>Затронутых рядов: '.$xn.'</div><br>'; else $xn='';
		$first=true;
		if($db->num_rows())while($db->next_record()){
			if($first){
				echo '<h2>Результаты:</h2>'.$xn;
				echo '<table id="records" cellpadding="3" cellspacing="1"><tr>';
				foreach($db->Record AS $var=>$value) echo '<th>'.$var.'</th>';
				echo '</tr>';
				$first=false;
			}
			echo '<tr>';
			foreach($db->Record AS $var=>$value){
				echo '<td>'.$value.'</td>';
			}
			echo '</tr>';
		}
		if(!$first) echo '</table>';
		else echo '<h2>Запрос вернул пустой результат</h2>'.$xn;
	}
	if($lang=='php' && !empty($code_php)){
		echo '<h2>Результат запроса:</h2>';
		echo '<div style="padding: 10px; background-color: #F9F9FF; border-top: 1px solid #1076DC; border-bottom: 2px solid #1076DC;">';
		eval($code_php);
		echo '</div>';
	}
	if($lang=='e5' && !empty($code_e5)){
		echo '<h2>Результат запроса:</h2>';
		echo '<div style="padding: 10px; background-color: #F9F9FF; border-top: 1px solid #1076DC; border-bottom: 2px solid #1076DC;">';
		if($user_e5!=-1){
			if($user_e5==0) $rs=backup_globals('guest',false);
			else $rs=backup_globals($user_e5,false);
		}
		echo shell_tpl($code_e5);
		if(isset($rs)) return_globals($rs);
		echo '</div>';
	}
	if($lang=='terminal' && !empty($code_terminal)){
		$code_terminal=escapeshellcmd($code_terminal);
		echo '<h2>Результат запроса:</h2>';
		echo '<div style="padding: 10px; background-color: #F9F9FF; border-top: 1px solid #1076DC; border-bottom: 2px solid #1076DC;">';
		$res=shell_exec($code_terminal);
		//$res=mb_convert_encoding($res,'UTF-8',mb_detect_encoding($res,'CP866'));
		echo $res;
		echo '</div>';
	}
}

if(!empty($action) && $action=='kill' && !empty($id)){
	shell_exec('kill '.$id);
	echo '<h2>Процесс с номером '.$id.' удалён</h2>';
}

echo '<div style="'.($lang!='process'?'display: none;':'').'" id="code_process">';
$data=Array();
$nr='';
$mpid=getmypid();
if(!empty($prc) && count($prc)>1) foreach($prc AS $tm){
    $tm=explode(' ',trim(clear_dblspace($tm)));
    if(isset($tm[10])){
        if(empty($data[$tm[10]])) $data[$tm[10]]=Array();
        $n=count($data[$tm[10]]);
        $data[$tm[10]][$n]=Array();
        $data[$tm[10]][$n]["id"]=$tm[1];
        $data[$tm[10]][$n]["cpu"]=$tm[2];
        $data[$tm[10]][$n]["mem"]=$tm[3];
        $data[$tm[10]][$n]["time"]=$tm[9];
        $data[$tm[10]][$n]["time2"]=$tm[8];
        if($tm[1]==$mpid) $nr=$tm[10];
    }
}
if($nr!='' && !empty($data[$nr])){
	echo '<table cellpadding="5" cellspacing="3" id="records"><tr>
		<th>ID</th>
		<th>Загрузка ЦПУ</th>
		<th>Потребляемая память</th>
		<th>Время работы</th>
		<th>Домен/URL</th>
		<th>IP вызывающего</th>
		<th>Действия</th></tr>';
	foreach($data[$nr] AS $o){
		$o2=extract_from_log($o["id"],$o["time2"]);
		if($o2){
			$domain=$o2["domain"].' '.$o2["url"];
			$ip=$o2["ip"];
		} else {
			$domain="";
			$ip="";
		}
		echo '<tr>';
		echo '<td>'.$o["id"].'</td>';
		echo '<td>'.$o["cpu"].'</td>';
		echo '<td>'.$o["mem"].'</td>';
		echo '<td>'.$o["time"].'</td>';
		if($o["id"]==$mpid) echo '<td>Текущий процесс</td>';
		else echo '<td>'.$domain.'</td>';
		echo '<td>'.$ip.'</td>';
		echo '<td><a href="terminal?action=kill&lang=process&proc='.$o["id"].'">Удалить</a></td>';
		echo '</tr>';
	}
	echo '</table>';
	echo '<h2>Информация о нагрузке за последний час</h2>';
	global $pl_temp_pid2;
	$domains=Array();
	$ips=Array();
	$visitors=0;
	if(!empty($pl_temp_pid2)) foreach($pl_temp_pid2 AS $id=>$elem_all) foreach($elem_all AS $data=>$elem){
		$visitors++;
		$d=$elem["domain"];
		$d=str_replace('www.','',$d);
		if(empty($domains[$d])) $domains[$d]=1;
		else $domains[$d]++;
		if(empty($ips[$elem["ip"]])) $ips[$elem["ip"]]=1;
		else $ips[$elem["ip"]]++;
	}
	echo '<table id="records" cellpadding="5" cellspacing="3">';
	echo '<tr><td>Затронуто доменов:</td><td>'.count($domains).'</td></tr>';
	echo '<tr><td>Хосты:</td><td>'.count($ips).'</td></tr>';
	echo '<tr><td>Визиты:</td><td>'.$visitors.'</td></tr>';
	if(count($ips)>0) echo '<tr><td>Визитов на хост:</td><td>'.floor($visitors/count($ips)).'</td></tr>';
	echo '</table>';
	echo '<h2>Топ 10 доменов</h2><table id="records" cellpadding="5" cellspacing="3"><tr><th>Домен</th><th>Визитов</th></tr>';
	arsort($domains);
	$cont=0;
	foreach($domains AS $name=>$count){
		$cont++;
		echo '<tr><td>'.$name.'</td><td>'.$count.'</td></tr>';
		if($cont==10) break;
	}
	echo '</table>';
	echo '<h2>Топ 10 IP</h2><table id="records" cellpadding="5" cellspacing="3"><tr><th>IP</th><th>Визитов</th></tr>';
	arsort($ips);
	$cont=0;
	foreach($ips AS $name=>$count){
		$cont++;
		echo '<tr><td>'.$name.'</td><td>'.$count.'</td></tr>';
		if($cont==10) break;
	}
	echo '</table>';
}
echo '</div>';

?>