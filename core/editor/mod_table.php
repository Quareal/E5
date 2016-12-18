<?php

//PREPEND
global $id,$id2,$id6,$id7;
if(empty($_GET["id"]) && empty($_POST["id"])) {include('main.php'); exit;}
if(empty($_GET["id2"]) && empty($_POST["id2"])) {include('main.php'); exit;}
if(empty($id6)) $id6=0;
//id = module_id
//id2 = table_ex
//id6 = owner_row
//id7 = sub_table for owner_row
getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,'main_module');
if(empty($db->Record)){ include('modules.php'); exit;}
foreach($db->Record AS $var=>$value) $$var=$value;
if(!check_mod($id,'view')){include('main.php'); exit;}
getrow($db,"SELECT * FROM ex_table WHERE ex_id=$id2",1,'ex_table');
foreach($db->Record AS $var=>$value) $$var=$value;
$ex_table2=$ex_table; if(!empty($id7)) $ex_table2=$id7;

$max_rows=5000; // Максимальное кол-во строк с проверкой прав доступа, если значение превышено - пользователь видит общий объём строк, но не видит сами строки

if(!check_tbl(get_st($id6,$ex_table2),'view')){
	include('main.php'); exit;
}

//News update
if(!empty($nid)){
	global $user;
	$n=get_news(0,0,-1,-2,-2,0,$nid);
	if($n){
		global $db,$user;
		getrow($db,"UPDATE main_news SET news_read=$user->id WHERE news_id=$nid",3,"main_news");
	}
}

$exs2=getall($db,"SELECT * FROM ex_group WHERE ex_ex1=".$id2,1,'ex_group');
$bool=false;
foreach($exs2 AS $exs){
	foreach($exs AS $var=>$value) $$var=$value;
	$ex_ex2b=$ex_ex2;
	$ex_ex2v=$ex_ex2;
	if(check_ex($ex_ex2,'view',0)){$bool=true; break;}
}
if(!$bool){include('main.php'); exit;}

$exs2=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id",1,"ex_module");$exs=Array();
foreach($exs2 AS $tmp) $exs[$tmp["ex_id"]]=$tmp;
global $start_url, $start_url2;
$start_url=get_base_url($id2,$id6);
$start_url2=get_base_url($id2,getrowval("SELECT * FROM row_owner WHERE owner_id=$id6 AND ro_ex=$id2","row_id"),1);

if(!empty($start_url2) && $start_url2[strlen($start_url2)-1]!='/') $start_url2.='/';

// PREPEND
$mrow=0;
global $rlink,$rolink,$cum;
if(empty($id6)){
	$cum=$id;
	getrow($db,"SELECT * FROM main_table WHERE table_id=".$ex_table,1,'main_table');
	if(empty($db->Record)){
		include('main.php'); exit;
	}
	foreach($db->Record AS $var=>$value) $$var=$value;
	$nm=$table_module;
	$texs=getall($db,"SELECT * FROM ex_group WHERE ex_module=$id AND ex_table=$table_id",1,"ex_group");
	if($use_titles) {
		echo '<h1>Редактировать таблицу «'.$table_name.'» ('.$ex_name.')</h1>';
		echo '<h2 align="center">Модуль «'.$module_name.'»</h2>';
	}
	echo '<table width="100%" cellpadding="0" cellspacing="0" style="margin-bottom: 10px;"><tr><td>';
	$sel1='';
	echo '</td><td align="right">';
	//echo '<div align="right"><a href="mod_main?id='.$id.'">Назад к модулю «'.$module_name.'»</a></div>';
	//echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
	//echo '<br>';
	//if(check_mod($id,"edit"))echo '<div align="right"><a href="mod_col?id='.$id.'&amp;id2='.$ex_table.'">Переменные таблицы</a></div>';
	//if(check_ex($ex_ex2,"edit"))echo '<div align="right"><a href="mod_main?id='.$id.'&id2='.$ex_ex2.'&action=edit_ex_form#edit_ex">Изменить экземпляр</a></div>';
	if(count($exs)>1){
		//echo '<div align="right">Экземпляр: ';
		$sel1.='<select name="fast_go" class="button" OnChange="document.location=\'mod_table?id='.$id.'&id2=\'+this.value;">';
		$seld=Array();
		foreach($texs AS $tex)if(check_ex($tex["ex_ex2"],'view',0)){
			$sel='';
			if($tex["ex_ex1"]==$id2){ $sel=' selected'; $seld[$tex["ex_ex2"]]=$tex["ex_ex1"]; }
			if(!empty($exs[$tex["ex_ex2"]])) $tt=$exs[$tex["ex_ex2"]]["ex_name"]; else $tt='';
			if(strlen5($tt)>60) $tt=substr($tt,0,60).'…';
			$sel1.='<option value="'.$tex["ex_ex1"].'"'.$sel.'>'.$tt.'</option>';
		}
		if(count($seld)>1 && !empty($_COOKIE["cex".$id]) && isset($seld[$_COOKIE["cex".$id]])){
			$sel1=str_replace(' selected>','>',$sel1);
			getrow($db,"SELECT * FROM ex_module WHERE ex_id=".$_COOKIE["cex".$id],1,"ex_module");
			$sel1=str_replace('>'.$db->Record["ex_name"].'<',' selected>'.$db->Record["ex_name"].'<',$sel1);
		}
		$sel1.='</select>';
		//echo '</div>';
	}
	
	$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$id AND (table_bold!=2 OR table_id=$table_id)",1,"main_table");
	$texs2=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$ex_ex2",1,"ex_group");
	if(!empty($texs2)) foreach($texs2 AS $txs) $texs[$txs["ex_table"]]=$txs["ex_ex1"];
	$tbls2=Array();
	foreach($tbls AS $tb) if(check_tbl($tb["table_id"],'view',$id) && isset($texs[$tb["table_id"]])) $tbls2[]=$tb;
	if(count($tbls2)>1){
		$sel2='<select style="width: 120px;" name="fast_go" class="button" OnChange="document.location=\'mod_table?id='.$id.'&id2=\'+this.value;">';
		foreach($tbls2 AS $tbl){
			$sel='';
			if($tbl["table_id"]==$table_id) $sel=' selected';
			$sel2.='<option value="'.$texs[$tbl["table_id"]].'"'.$sel.'>'.$tbl["table_name"].'</option>';
		}
		$sel2.='</select>';
	}

	if(!empty($sel2)) echo se('select_table','','Выбрать другую таблицу','style="margin-left: 30px;" align="absmiddle"',0).$sel2;	
	if(!empty($sel1)) echo se('back2','','Выбрать другой раздел','style="margin-left: 30px;" align="absmiddle"',0).$sel1;
	
	echo se('back_module','mod_main?id='.$id,'Назад к модулю','style="margin-left: 30px;"');
	echo se('back_modules','modules','Назад к списку модулей');
	
	$fee=false;
	if(check_mod($nm,"edit")){
		$fee=true;
		echo se('back_table','mod_col?id='.$id.'&amp;id2='.$ex_table,'Переменные таблицы',' style="margin-left: 20px;"',1,0);
	}
	//if(check_row($id6,$table_id,$ex_ex2,"edit")){
	if(!empty($start_url)){
		if($fee) $mrg='10'; else $mrg='20';
		echo se('anchor',$start_url,'Ссылка на раздел',' target="_blank" style="margin-left: '.$mrg.'px;"',1,0);
		$fee=true;
	}
	if(check_ex($ex_ex2,'edit')){
		if($fee) $mrg='10'; else $mrg='20';
		echo se('back_config','mod_main?id='.$id.'&id2='.$ex_ex2.'&action=edit_ex_form#edit_ex','Изменить раздел',' style="margin-left: '.$mrg.'px;"',1,0);
	}	
	
	echo '</td></tr></table>';
	
	global $ctbl;
	$ctbl=$table_id.':'.$ex_ex2.':0:'.$table_sname.':'.$ex_ex1;
} else {
	getrow($db,"SELECT * FROM main_table WHERE table_id=".$id7,1,'main_table');
	foreach($db->Record AS $var=>$value) $$var=$value;
	$nm=$table_module;
	$cum=$nm;
	getrow($db,"SELECT * FROM row_owner WHERE row_id=".$id6,1,'row_owner');
	$up_table_id=$db->Record["row_table"];
	function seek_mparent($row_id,$table_id){
		global $db;
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$row_id",1,'row_owner');
		if(!empty($db->Record)){
			if($table_id!=$db->Record["owner_table"]) return $db->Record["owner_id"];
			else if($db->Record["owner_id"]==0) return 0;
			else return seek_mparent($db->Record["owner_id"],$table_id);
		}
	}
	$mrow=seek_mparent($id6,$up_table_id);
	getrow($db,"SELECT * FROM main_table WHERE table_id=".$up_table_id,1,'main_table');
	foreach($db->Record AS $var=>$value) {$var.='2'; $$var=$value;}
	if(empty($tmj[$table_id2])){
		getrow($db,"SELECT * FROM main_table WHERE table_id=$table_id2",1,'main_table');
		if($db->Record["major_col"]!=0){
			getrow($db,"SELECT * FROM main_col WHERE col_id=".$db->Record["major_col"],1,'main_col');
			$tmj[$table_id2]=$db->Record;
		}else{
			getrow($db,"SELECT * FROM main_col WHERE col_table=$table_id2 AND col_bold=1 ORDER BY col_pos LIMIT 1",1,'main_col');
			if(!empty($db->Record)) $tmj[$table_id2]=$db->Record;
		}
	}
	if(!empty($tmj[$table_id2]["col_id"])){
		getrow($db,"SELECT * FROM row_value WHERE value_row=$id6 AND value_col=".$tmj[$table_id2]["col_id"],1,'row_value');
		$owner_row=$db->Record["value_value"];
	} else $owner_row='';
	if($use_titles){
		echo '<h1>Редактировать таблицу «'.$table_name.'»<br><span style="font-size: 12px;">(вложенную в строку «'.$owner_row.'» таблицы «'.$table_name2.'» - '.$ex_name.')</span></h1>';
		echo '<h2 align="center">Модуль «'.$module_name.'»</h2>';
	}
	
	echo '<table width="100%" cellpadding="0" cellspacing="0" style="margin-bottom: 10px;"><tr><td>';
	

	$sel1='';$sel2='';
	$rr=get_vars2(get_sub($mrow,$up_table_id,0,1,0,0,0,$ex_ex1,$ex_ex2,$up_table_id,0,$up_table_id,1));
	if(count($rr)>1 || (count($rr)==1 && !empty($rr[0]->sub))){
		$sel1='<select style="width: 120px;" name="fast_go" class="button" OnChange="document.location=\'mod_table?id='.$id.'&id2='.$id2.'&id7='.$id7.'&id6=\'+this.value;">';
		$sel1.=options($rr,'',$id6,1,0,0,0,'',Array(),60);
		$sel1.='</select>';
	}
	
	$tbls=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$table_id2",1,"table_sub");
	$w='';
	if(!empty($tbls) && count($tbls)>1) foreach($tbls AS $tbl){if($w!='') $w.=','; $w.=$tbl["sub_table2"];}
	if(!empty($w)){
		$tbls=getall($db,"SELECT * FROM main_table WHERE table_id IN ($w)",1,"main_table");
		if(count($tbls)>1){
			$sel2='<select style="width: 120px;" name="fast_go" class="button" OnChange="document.location=\'mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7=\'+this.value;">';
			foreach($tbls AS $tbl){
				$sel='';
				if($tbl["table_id"]==$id7) $sel=' selected';
				$sel2.='<option value="'.$tbl["table_id"].'"'.$sel.'>'.$tbl["table_name"].'</option>';
			}
			$sel2.='</select>';
		}
	}
	
	echo '</td><td align="right">';

	if(!empty($sel2)) echo se('select_table','','Выбрать другую таблицу','style="margin-left: 30px;" align="absmiddle"',0).$sel2;
	if(!empty($sel1)) echo se('back2','','Выбрать другого родителя','style="margin-left: 30px;" align="absmiddle"',0).$sel1;
	
	echo se('back','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$mrow.'&amp;id7='.$up_table_id,'',' style="margin-left: 30px;"');
	echo se('back_module','mod_main?id='.$id,'Назад к модулю');
	echo se('back_modules','modules','Назад к списку модулей');

	$fee=false;
	if(check_mod($nm,"edit")) {
		$fee=true;
		echo se('back_table','mod_col?id='.$nm.'&amp;id2='.$id7,'Переменные таблицы',' style="margin-left: 20px;"',1,0);
	}
	if(!empty($start_url)){
		if($fee) $mrg='10'; else $mrg='20';
		echo se('anchor',$start_url,'Ссылка на раздел',' target="_blank" style="margin-left: '.$mrg.'px;"',1,0);
		$fee=true;
	}
	seek_rlink($id6);
	if(check_row($id6,$table_id,$ex_ex2,"edit",$rlink[$id6]->user,$rlink[$id6]->users)){
		if($fee) $mrg='10'; else $mrg='20';
		$id6u=0;
		$id7u=0;
		$ownr=$id6;
		$old_ownr=$ownr;
		if(!empty($ownr)) seek_rlink($ownr);
		while(!empty($ownr) && !empty($rlink[$ownr])){
			if(isset($ch_table) && $ch_table!=$rlink[$ownr]->table){
				$id6u=$ownr;
				$id7u=$rlink[$old_ownr]->table;
				break;
			}
			if(!isset($ch_table)){
				$ch_table=$rlink[$ownr]->table;
				$old_ownr=$ownr;
			}
			
			$ownr=$rlink[$ownr]->owner;
			if(!empty($ownr)) seek_rlink($ownr);
		}
		echo se('back_config','mod_table?id='.$id.'&id2='.$id2.'&id3='.$id6.'&id4=0&id5='.$rlink[$id6]->rid.'&id6='.$id6u.'&id7='.$id7u.'&action=edit_form#editform','Изменить раздел',' style="margin-left: '.$mrg.'px;"',1,0);
	}
	
	
	echo '</td></tr></table>';

	//$tid=$table.':'.$ex.':'.$owner.':'.$sname.':'.get_tex($owner,$ex,$table);
	global $ctbl;
	$ctbl=$table_id.':'.$ex_ex2.':'.$id6.':'.$table_sname.':'.$ex_ex1;
}
global $cur_module;
$cur_module=$cum;
if(empty($GLOBALS["buff"]) && !empty($_COOKIE["buff"])) $GLOBALS["buff"]=$_COOKIE["buff"];
$cur_buff=Array();
if(!empty($GLOBALS["buff"])){
	$tmp=explode('**',$GLOBALS["buff"]);
	if($tmp[0]==$table_id){
		$tmp=explode(';',$tmp[1]);
		if(!empty($tmp)) foreach($tmp AS $t){
			$t=explode(':',$t);
			$cur_buff[$t[1]]=$t[1];
		}
	}
}

if(!empty($action)) $old_action=$action; else $old_action='';


//Подготовка фильтра
$fcols=getall4($db,"SELECT * FROM main_col WHERE col_table=$table_id AND col_filter!=0","col_id");
if(!empty($sbt) && $sbt=='Отмена'){
	unset($f);
	if(isset($_POST["f"])) unset($_POST["f"]);
	if(isset($_GET["f"])) unset($_GET["f"]);
}
if(!empty($f) && !is_array($f)){
	$f=urldecode($f);
	//echo $f.'<br>';
	$tf=$f;
	$f=Array();
	$tf=explode('&&',$tf);
	$uc=Array();
	$wh=Array();
	foreach($tf AS $tff){
		$tff=explode('^^',$tff);
		$fid=$tff[0];
		if(strlen($tff[1])>1 && $tff[1][0]=='~'){
			$x=$tff[1][1];
			$tff[1]=substr5($tff[1],2);
			$uc[$fid]=1;
			$wh[$fid]=1;
			//echo $fid.'<br>';
			if($x==1 || $x==3 || $x==5) $uc[$fid]=2;
			if($x==2 || $x==3) $wh[$fid]=2;
			if($x==4 || $x==5) $wh[$fid]=3;
		}
		if(!empty($fcols[$fid]) && $fcols[$fid]["col_filter"]==3){
			$tmp=explode('**',$tff[1]);
			$tff[1]=Array();
			foreach($tmp AS $tm) $tff[1][$tm]=1;
		}
		$f[$fid]=$tff[1];
	}
	$_GET["f"]=$f;
	$_POST["f"]=$f;
}
$df='';$df2='';$df3='';
if(!empty($f)){//компоновка нужна сразу после декодирования (а то потом вдруг захочется это удалить, т.к. декодирование происходит не всегда
	$df=Array();
	foreach($f AS $fid=>$tf){	
		$s=$fid.'^^';
		if(isset($uc[$fid]) && !is_array($tf)/*мало ли*/){
			if($uc[$fid]==1 && $wh[$fid]==1) $tf='~0'.$tf;
			if($uc[$fid]==2 && $wh[$fid]==1) $tf='~1'.$tf;
			if($uc[$fid]==1 && $wh[$fid]==2) $tf='~2'.$tf;
			if($uc[$fid]==2 && $wh[$fid]==2) $tf='~3'.$tf;
			if($uc[$fid]==1 && $wh[$fid]==3) $tf='~4'.$tf;
			if($uc[$fid]==2 && $wh[$fid]==3) $tf='~5'.$tf;
			//echo $uc[$fid].' - '.$wh[$fid].'<br>';
		}
		if(is_array($tf)){
			$tmp=Array();
			foreach($tf AS $var=>$value) $tmp[$var]=$var;//в принципе, можно было сделать flip_array-ем
			$tf=implode('**',$tmp);
		}
		$s.=$tf;
		$df[]=$s;
	}
	$df=implode("&&",$df);
	if(!empty($df)){
		//echo $df;
		$df2='<input type="hidden" name="f" value="'.urlencode($df).'">';
		$df='&amp;f='.urlencode($df);
		$df3='&f='.urlencode($df);
	}
}


// Проверка на возможность добавления новых значений в таблицу
$can_add=false;
global $rlink;
if(!empty($id6)) seek_rlink($id6);//getrow($db,"SELECT * FROM main_row WHERE row_id=$id6");
if(	(empty($id6) && check_row(0,$ex_table2,$ex_ex2v,'add')) ||
	(!empty($id6) && check_row($id6,$rlink[$id6]->table,$ex_ex2v,'edit',$rlink[$id6]->user,$rlink[$id6]->users))) $can_add=true;

//=======================
//  Вход из ex_zone
//=======================
if(!empty($action) && $action=='setcz'){
	$cex=$ncm;
	SetCookie('cex'.$id,$ncm, time()+60*60*24*30,'/');
	SetCookie('cz',$ncz, time()+60*60*24*30,'/');
	$GLOBALS["cex".$id]=$ncm;
	$cz=$ncz;
	$action='';
}

//=======================
//  Загрузка объектов с сервера
//=======================
if(!empty($action) && $action=='upload'){
	include_once(DOCUMENT_ROOT.'/core/update/functions.inc');
	include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
	if(!empty($upload)){
		$rows=Array();
		//$components='';
		foreach($upload AS $row=>$checked) if(!empty($checked)){
			$rows[$row]=$row;
			/*if(!empty($install_components[$row])){
				if(!empty($components)) $components.='^^';
				$components.=$install_components[$row];
			}*/
		}
		/*if(!empty($components)){
			$components=explode('^^',$components);
			foreach($components AS $component) import_part($component);
		}*/
		//echo $GLOBALS["update_server"].'?type=get_rows&rows='.implode(',',$rows);
		$data=loadserv($GLOBALS["update_server"].'?type=get_rows&rows='.implode(',',$rows));
		$data=explode('_!^^!_',$data);
		start_export(0,false);
		text_to_rows($data[0],$id2,$id6,0,$id7);
		end_export();
		//компоненты
		if(!empty($data[1])){
			//echo $data[1];
			$rows=unserialize($data[1]);
			global $db;
			$tmp=getall($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2",1,"main_part");
			$parts_dst=Array();
			if(!empty($tmp)) foreach($tmp AS $t){
				$parts_dst[$t['part_proc']][$t['part_sname']]=1;
			}
			foreach($rows AS $row=>$components)
			foreach($components AS $part_proc=>$tmp)
			foreach($tmp AS $part_sname=>$t)
			if(empty($parts_dst[$part_proc][$part_sname])){
				//echo $part_proc.'::'.$part_sname.'<br>';
				import_part($part_proc.'::'.$part_sname);
			}
		}
		echo '<h2>Загрузка с сервера завершена</h2>';
	}
}

//=======================
//  Пересортировка
//=======================

function seek_double($owner){
	global $db;
	$res='';
	$tmp=getall($db,"SELECT * FROM row_owner WHERE owner_id=$owner",1,'row_owner');
	if(!empty($tmp)) foreach($tmp AS $tm){
		$res.=',';
		$res.=$tm["row_id"];
		$res.=seek_double($tm["row_id"]);
	}
	return $res;
}

if(!empty($action) && $action=='resort'){
	del_cache('rows',$table_id.'.'.$id2);
	if(!empty($chk)) foreach($chk AS $var=>$value){
		$var=explode(':',$var);
		del_cache('row',$var[1]);
	}
	if(!empty($fe))foreach($fe AS $row=>$tmp) if(!empty($tmp)){
		del_cache('row',$row);
	}
	if(!empty($pos)) foreach($pos AS $var=>$value){
		$db->query("UPDATE row_owner SET ro_pos=$value WHERE ro_id=$var",3,'row_owner');
	}
	if(!empty($bfe))foreach($bfe AS $row=>$tmp) if(!empty($tmp)) foreach($tmp AS $col=>$value) $db->query("UPDATE row_value SET value_value='0' WHERE value_row=$row AND value_col=$col",3,'row_value');
	if(!empty($fe))foreach($fe AS $row=>$tmp) if(!empty($tmp)){
		//getrow($db,"SELECT * FROM main_row WHERE row_id=$row",1,"main_row");
		seek_rlink($row);
		$cols_prep=getall4($db,"SELECT col_id, col_type, col_table, col_module FROM main_col WHERE col_table=$table_id AND col_module=$id","col_id");
		//if(empty($db->Record["row_user"])) $db->Record["row_user"]=0;
		if(!empty($rlink[$row]) && isset($rlink[$row]->user) && check_row($row,$ex_table2,$ex_ex2v,'edit',$rlink[$row]->user,$rlink[$row]->users)) foreach($tmp AS $col=>$value){
			if(!empty($bfe[$row][$col])) $value=1;
			$db->query("SELECT * FROM row_value WHERE value_row=$row AND value_col=$col",1,"row_value");		
			/*$value=str_replace("\\\\","|-|-|-|-|-|",$value);// возможно поможет для ликвидации нижеследующего еррора
			$value=str_replace("''","=!=!=!=",$value);
			$value=str_replace("'","''",$value);	// может вызывать серьёзные ошибки, если значение уже прошло обработку для постинга
			$value=str_replace("\\","\\\\",$value);
			$value=str_replace("|-|-|-|-|-|","\\\\",$value);// возможно поможет для ликвидации нижеследующего еррора
			$value=str_replace("=!=!=!=","''",$value);	*/
			if(isset($cols_prep[$col]) && $cols_prep[$col]["col_type"]==2 && $value=='on') $value=1;
			if($db->num_rows()){
				$db->query("UPDATE row_value SET value_value='$value' WHERE value_row=$row AND value_col=$col",3,'row_value');
			} else {
				$db->query("INSERT INTO row_value (value_value, value_row, value_col, value_module, value_table)
					VALUES('$value',$row,$col,$id,$table_id)",3,'row_value');
			}
			$check_rows[$row]=$row;
		}
		if(!empty($check_rows)) update_row_state($check_rows);
	}
	if($chk_type==1 && !empty($chk))foreach($chk AS $var=>$value)if(!empty($value)){		
		$var=explode(':',$var);
		//getrow($db,"SELECT * FROM row_owner WHERE ro_id=".$var[0],1,"row_owner");		
		//if(empty($db->Record["ro_user"])) $db->Record["ro_user"]=0;
		//if(empty($db->Record["row_id"])) $db->Record["row_id"]=0;//? <- WTF?
		seek_rlink2($var[0]);
		if(!empty($rolink[$var[0]]) && check_row($rlink[$rolink[$var[0]]]->id,$ex_table2,$ex_ex2v,'del',$rlink[$rolink[$var[0]]]->user,$rlink[$rolink[$var[0]]]->users)){
			$rows=getall($db,"SELECT * FROM row_owner WHERE row_id=".$var[1]." LIMIT 2",1,'row_owner');
			if(count($rows)>1) $db->query("DELETE FROM row_owner WHERE ro_id=".$var[0],1,'row_owner');
			else del_row($var[1]);
		}
	}
	if($chk_type==2 && !empty($chk))foreach($chk AS $var=>$value)if(!empty($value)){
		$var=explode(':',$var);
		//getrow($db,"SELECT * FROM row_owner WHERE ro_id=".$var[0],1,"row_owner");		
		//if(empty($db->Record["ro_user"])) $db->Record["ro_user"]=0;	
		seek_rlink2($var[0]);
		if(!empty($rolink[$var[0]]) && check_row($rlink[$rolink[$var[0]]]->id,$ex_table2,$ex_ex2v,'edit',$rlink[$rolink[$var[0]]]->user,$rlink[$rolink[$var[0]]]->users)){		
		//if(!empty($db->Record) && check_row($db->Record["row_id"],$ex_table2,$ex_ex2v,'edit',$db->Record["ro_user"])){
			$db->query("UPDATE row_owner SET ro_enable=1 WHERE ro_id=".$var[0],3,"row_owner");
			getrow($db,"SELECT * FROM row_owner WHERE row_id=".$var[1]." AND ro_enable=0",1,"row_owner");
			if(empty($db->Record)) $db->query("UPDATE main_row SET row_enable=1 WHERE row_id=".$var[1],3,"main_row");
		}
	}
	if($chk_type==3 && !empty($chk))foreach($chk AS $var=>$value)if(!empty($value)){
		$var=explode(':',$var);
		//getrow($db,"SELECT * FROM row_owner WHERE ro_id=".$var[0],1,"row_owner");		
		//if(empty($db->Record["ro_user"])) $db->Record["ro_user"]=0;
		//if(empty($db->Record["row_id"])) $db->Record["row_id"]=0;//? <- WTF?
		//if(check_row($db->Record["row_id"],$ex_table2,$ex_ex2v,'edit',$db->Record["ro_user"])){
		seek_rlink2($var[0]);
		if(!empty($rolink[$var[0]]) && check_row($rlink[$rolink[$var[0]]]->id,$ex_table2,$ex_ex2v,'edit',$rlink[$rolink[$var[0]]]->user,$rlink[$rolink[$var[0]]]->users)){		
			$db->query("UPDATE row_owner SET ro_enable=0 WHERE ro_id=".$var[0],3,"row_owner");
			getrow($db,"SELECT * FROM row_owner WHERE row_id=".$var[1]." AND ro_enable=1",1,"row_owner");
			if(empty($db->Record)) $db->query("UPDATE main_row SET row_enable=0 WHERE row_id=".$var[1],3,"main_row");
		}
	}
	if($chk_type==4 && !empty($id6)){
		$chk_type=5;
		$new_own=$new_ownB;
		$old_chk_type=4;
	}
	if($chk_type==10 && !empty($chk)){
		include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
		$rows=Array();
		foreach($chk AS $var=>$value)if(!empty($value)){
			$var=explode(':',$var);
			$rows[$var[0]]=$var[1];
		}
		start_export();
		$import=rows_to_text($rows);
		end_export();
		echo_file($import,$table_name.' '.date('Y-m-d').'.e5');
	}
	if($chk_type==4 && !empty($chk)){
		foreach($chk AS $var=>$value)if(!empty($value)){
			$var=explode(':',$var);
			$rows[$var[0]]=$var[1];
			$rows2[$var[1]]=$var[0];
		}
		function edit_subs($id){
			global $db,$new_ex;
			$db->query("UPDATE main_row SET row_ex=$new_ex WHERE row_id=$id",3,"main_row");
			$srs=getall($db,"SELECT * FROM row_owner WHERE owner_id=$id",1,"row_owner");
			if(!empty($srs)) foreach($srs AS $sr){
				$db->query("UPDATE row_owner SET ro_ex=$new_ex WHERE ro_id=".$sr["ro_id"],3,"row_owner");
				edit_subs($sr["row_id"]);
			}
		}
		foreach($rows2 AS $row){
			/*$db->query("SELECT * FROM row_owner WHERE ro_id=".$row,1,"row_owner");	
			if(empty($db->Record["ro_user"])) $db->Record["ro_user"]=0;
			if(empty($db->Record["row_id"])) $db->Record["row_id"]=0;//? <- WTF?
			if(empty($db->Record["owner_id"])) $db->Record["owner_id"]=0;*/
			
			seek_rlink2($row);
			if(!empty($rolink[$row])) if(($rlink[$rolink[$row]]->owner==0 || empty($rows[$rlink[$rolink[$row]]->owner])) && check_row($rlink[$rolink[$row]]->id,$ex_table2,$ex_ex2v,'edit',$rlink[$rolink[$row]]->user,$rlink[$rolink[$row]]->users)){			
			//if(!empty($db->Record))if(($db->Record["owner_id"]==0 || empty($rows[$db->Record["owner_id"]]))  && check_row($db->Record["row_id"],$ex_table2,$ex_ex2v,'edit',$db->Record["ro_user"])) {
				$db->query("UPDATE row_owner SET ro_ex=$new_ex, owner_id=0 WHERE ro_id=$row",3,"row_owner");
				edit_subs($rows[$row]);
			}
		}
		flush_cache();
	}
	//смена владеющего юзера
	if($chk_type==9 && !empty($chk) && ($new_own2==0 || ($new_own2==-1 && $user->super) || check_user(-$new_own2,'view'))){
		foreach($chk AS $var=>$value)if(!empty($value)){
			$var=explode(':',$var);
			$row=$var[1];//main row
			global $rlink;
			seek_rlink($row);
			$r=$rlink[$row];
			if(check_row($r->id,$r->table,$ex_ex2v,'edit',$r->user,$r->users)){
				$db->query("UPDATE row_owner SET ro_user=$new_own2 WHERE row_id=$row",3,"row_owner");
				$db->query("UPDATE main_row SET row_user=$new_own2 WHERE row_id=$row",3,"main_row");
			}
			unset($rlink[$row]);
		}
	}
	if($chk_type==5 && !empty($chk)){
		foreach($chk AS $var=>$value)if(!empty($value)){
			$var=explode(':',$var);
			$rows[$var[0]]=$var[1];//main row
			$rows2[$var[1]]=$var[0];//row owner
		}
		//$new_own  - main_row
		if($new_own!=0){
			getrow($db,"SELECT * FROM main_row WHERE row_id=".$new_own,1,"main_row");
			$now=$db->Record;
			$nom=$now["row_module"];
			$not=$now["row_table"];
		} else {
			$nom=0; $not=0;
			if(!empty($id6)){
				$new_own=$id6;
				$not=$rlink[$id6]->table;
				$nom=$rlink[$id6]->module;
			}
		}
		$err=0;
		foreach($rows2 AS $row){
			$rid=$rows[$row];
			//$db->query("SELECT * FROM row_owner WHERE ro_id=".$row,1,"row_owner");

			seek_rlink2($var[0]);
			$child=explode(',',substr(seek_double($rid),1));	
			//if(check_row($rid,$ex_table2,$ex_ex2v,'edit',$db->Record["ro_user"]) && !in_array($new_own,$child) && $new_own!=$rid){
			if(check_row($rid,$ex_table2,$ex_ex2v,'edit',$rlink[$rolink[$var[0]]]->user,$rlink[$rolink[$var[0]]]->users) && !in_array($new_own,$child) && $new_own!=$rid){
				//echo $new_own;
				$add_sql='';
				if(!empty($old_chk_type) && $old_chk_type==4)  $add_sql="ro_sub=$new_own,";
				$db->query("UPDATE row_owner SET
						owner_id=$new_own,
						owner_module=$nom, ".$add_sql."
						owner_table=$not
					 WHERE ro_id=$row",3,"row_owner");
			} else $err++;
			//$new_own
		}
		if($err>0) echo '<p>У некоторых строк невозможно поменять родителя (строк: '.$err.')</p>';
		flush_cache();
	}
	// выполнение части над строками
	if($chk_type==6 && !empty($chk)){
		getrow($db,"SELECT * FROM main_part WHERE part_id=$part AND part_table=$table_id AND part_type=5",1,"main_part");
		if(!empty($db->Record["part_id"])){
			foreach($chk AS $var=>$value)if(!empty($value)){
				$var=explode(':',$var);
				$id3=$var[1];
				//getrow($db,"SELECT * FROM main_row WHERE row_id=".$id3);
				seek_rlink($id3);
				//if(!empty($db->Record) && check_row($id3,$ex_table2,$ex_ex2/*v*/,'edit',$db->Record["row_user"])){
				if(!empty($rlink[$id3]) && check_row($id3,$ex_table2,$ex_ex2/*v*/,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
					del_cache('row',$id3);
					echo shell_part($part,$ex_ex2/*v*/,$id3);
					unset($action);
					$check_rows[$id3]=$id3;
				}
			}
			if(!empty($check_rows)) update_row_state($check_rows);
		}
	}
	// копирование строк в буфер обмена и добавление к уже существующему буферу
	if(($chk_type==7 || $chk_type==8) && !empty($chk)){
		$gl='';
		$first=true;
		if($chk_type==8){ $gl=$GLOBALS["buff"/*.$table_id*/]; $gl=explode('**',$gl); $gl=$gl[1]; $first=false;}
		foreach($chk AS $var=>$value)if(!empty($value)){
			$tmp=explode(':',$var);
			$cur_buff[$tmp[1]]=$tmp[1];
			if(!$first) $gl.=';';
			$gl.=$var;
			$first=false;
		}
		$gl=$table_id.'**'.$gl;
		SetCookie('buff'/*.$table_id*/,$gl, time()+60*60*24*30,'/');
		$GLOBALS["buff"/*.$table_id*/]=$gl;
	}
	//вставка из буфера в подтаблицы
	if($chk_type==11 && !empty($chk) && !empty($selbuff) && !empty($GLOBALS["buff"])){
		$b=explode('**',$GLOBALS["buff"]);
		$dest_table=$b[0];
		if($b[0]==$selbuff && !empty($b[1])){
			$b=explode(';',$b[1]);			
			foreach($chk AS $dest_row=>$checked)if(!empty($checked)){
				$dest_row=explode(':',$dest_row);
				$dest_row_id=$dest_row[1];
				seek_rlink($dest_row_id);
				foreach($b AS $source_row){
					$source_row=explode(':',$source_row);
					$source_row_id=$source_row[1];
					$source_ro_id=$source_row[0];
					copy_row($source_row_id,$source_ro_id,$dest_row_id,$rlink[$dest_row_id]->tex,$rlink[$dest_row_id]->module,$dest_table,$rlink[$dest_row_id]->table,$rlink[$dest_row_id]->module,1);
				}
			}
		}
	}
	//редактирование полей
	if($chk_type==12 && !empty($chk) && !empty($selcol)/* && !empty($_POST["fastedit_col".$selcol])*/){
		$col=getrow($db,"SELECT * FROM main_col WHERE col_id=$selcol");
		if(!empty($_POST["fastedit_col".$selcol])) $value=$_POST["fastedit_col".$selcol];
		else $value='';
		foreach($chk AS $dest_row=>$checked)if(!empty($checked)){
			$dest_row=explode(':',$dest_row);
			$dest_row_id=$dest_row[1];
			$sid=$dest_row_id;
			seek_rlink($sid);
			if(check_row($sid,$ex_table2,$ex_ex2v,'edit',$rlink[$sid]->user,$rlink[$sid]->users)){
				$db->query("DELETE FROM row_value WHERE value_row=$sid AND value_col=$selcol",3,"row_value");
				if(is_array($value)){foreach($value AS $value_row=>$value_value)if($value_value){
					$db->query("INSERT INTO row_value (value_row, value_module, value_table, value_col, value_value)
					VALUES ($sid, $module_id, $table_id, $selcol, '$value_row')",3,"row_value");
				}} else {
					if($col["col_type"]==2){
						if($value) $value=1; else $value=0;
					}
					$db->query("INSERT INTO row_value (value_row, value_module, value_table, value_col, value_value)
					VALUES ($sid, $module_id, $table_id, $selcol, '$value')",3,"row_value");
				}
				del_cache('row',$dest_row_id);
				$check_rows[$sid]=$sid;
			}			
			
			
			/*foreach($_POST AS $var=>$value){
				if(strstr($var,'fastedit_col') && !strstr($var,'fastedit_col'.$selcol)) unset($_POST[$var]);
			}
			if(!empty($_FILES)) foreach($_FILES AS $var=>$value){
				if(strstr($var,'fastedit_col') && !strstr($var,'fastedit_col'.$selcol)) unset($_FILES[$var]);
			}
			if(check_operation('edit',$sid,$id6,$ex_ex2v,$ex_table2)){
				$backup=backup_row($dest_row_id);
				$GLOBALS["backrow"]=$backup;
				$GLOBALS["cancel"]=false;
				$GLOBALS["f_action"]='edit';
				
				$GLOBALS["cur_ex"]=$ex_ex2;
				$table_id3=$table_id;
				if(!empty($id6) && empty($ro_owner)){
					$ro_owner3=$id6;
					$table_id3=$up_table_id;
				}
				del_vals_pre($dest_row_id,$table_id);
				
				echo insert_values($table_id,$module_id,$dest_row_id,0,'fastedit_',0);
				
				if($GLOBALS["cancel"]){
					del_vals(" value_table!=0 AND value_row=$sid");
					copy_vars($backup,$sid);
					if($GLOBALS["cancel"]!=-1){
						if($GLOBALS["cancel"]==1) echo '<div align="center"><h2 style="color:#FF0000;">Изменение не удалось</h2></div>';
						else echo '<div align="center"><h2 style="color:#FF0000;">Изменение не удалось по причине: '.$GLOBALS["cancel"].'</h2></div>';
					}
					del_row($backup,1);
				} else del_row($backup,1);
				$GLOBALS["backrow"]=0;
				$GLOBALS["f_action"]='';
				del_cache('row',$dest_row_id);
			} else echo '<div align="center"><h2 style="color:#FF0000;">Недостаточно прав для завершения операции</h2></div>';*/
		}
		if(!empty($check_rows)) update_row_state($check_rows);
	}
	unset($chk);
}

//=======================
//  Работа с буфером обмена
//=======================
if(!empty($action) && $action=='buffer_move' && /*!empty($GLOBALS["buff".$table_id])*/ !empty($GLOBALS["buff"]) && strstr($GLOBALS["buff"],$table_id.'**') && $can_add){
	del_cache('rows',$table_id.'.'.$id2);
	//$aa=explode(';',$GLOBALS["buff".$table_id]);
	$tmp=explode('**',$GLOBALS["buff"]);
	$aa=explode(';',$tmp[1]);
	foreach($aa AS $var){
		$var=explode(':',$var);
		$rows[$var[0]]=$var[1];//main row
		$rows2[$var[1]]=$var[0];//row owner
	}
	$new_own=$id6;
	if($new_own!=0){
		getrow($db,"SELECT * FROM main_row WHERE row_id=".$new_own,1,"main_row");
		$now=$db->Record;
		$nom=$now["row_module"];
		$not=$now["row_table"];
	} else {
		$nom=$module_id; $not=$table_id;
	}
	foreach($rows2 AS $row){
		$rid=$rows[$row];
		//$db->query("SELECT * FROM row_owner WHERE ro_id=".$row,1,"row_owner");		
		//if(empty($db->Record["ro_user"])) $db->Record["ro_user"]=0;
		seek_rlink2($row);
		if(!empty($rolink[$row]) && check_row($rid,$ex_table2,$ex_ex2v,'edit',$rlink[$rolink[$row]]->user,$rlink[$rolink[$row]]->users) && $new_own!=$rid){
			//echo '<br><br>- main_row<br>row_ex='.$ex_ex2v.'<br>row_table='.$table_id.'<br>row_module='.$module_id.'<br>row_sub='.$id6.'<br>';
			$db->query("UPDATE main_row SET
					row_ex=$ex_ex1,
					row_table=$table_id,
					row_module=$module_id,
					row_sub=$id6
				 WHERE row_id=$rid",3,"main_row");
 			//echo '<br>- row_owner<br>ro_ex='.$ex_ex2v.'<br>row_table='.$table_id.'<br>row_module='.$module_id.'<br>ro_sub='.$id6.'<br><br>';
			$db->query("UPDATE row_owner SET
					ro_ex=$ex_ex1,
					ro_sub=$id6,
					row_table=$table_id,
					row_module=$module_id,
					owner_id=$new_own,
					owner_module=$nom,
					owner_table=$not
				 WHERE ro_id=$row",3,"row_owner");
		}
	}
	$action='buffer_clear';
}

if(!empty($action) && ($action=='buffer_copy' || $action=='buffer_copy2' || $action=='buffer_copy3') && /*!empty($GLOBALS["buff".$table_id])*/ !empty($GLOBALS["buff"]) && strstr($GLOBALS["buff"],$table_id.'**') && $can_add){
	del_cache('rows',$table_id.'.'.$id2);
	//$aa=explode(';',$GLOBALS["buff".$table_id]);
	$tmp=explode('**',$GLOBALS["buff"]);
	$aa=explode(';',$tmp[1]);
	$new_own=$id6;
	if($new_own!=0){
		getrow($db,"SELECT * FROM main_row WHERE row_id=".$new_own,1,"main_row");
		$now=$db->Record;
		$nom=$now["row_module"];
		$not=$now["row_table"];
	} else {
		$nom=$module_id; $not=$table_id;
	}
	foreach($aa AS $var){
		$var=explode(':',$var);
		$id3=$var[1];
		$id4=$var[0];
		//getrow($db,"SELECT * FROM main_row WHERE row_id=".$id3);
		//if(empty($db->Record["row_user"])) $db->Record["row_user"]=0;
		seek_rlink($id3);
		if(!empty($rlink[$id3]) && check_row($id3,$ex_table2,$ex_ex2v,'view',$rlink[$id3]->user,$rlink[$id3]->users)){
			//getrow($db,"SELECT * FROM row_owner WHERE ro_id=$id4",1,'row_owner');
			if($action=='buffer_copy') $bc=1;
			else if($action=='buffer_copy2') $bc=0;
			else if($action=='buffer_copy3') $bc=2;
			copy_row($id3,$id4,$id6,$ex_ex1,$module_id,$table_id,$not,$nom,$bc);
		}
	}
}

if(!empty($action) && $action=='buffer_del' && !empty($id3)){
	if(isset($cur_buff[$id3])) unset($cur_buff[$id3]);
	//удаление из буфера элемента (можно вынести в функцию)
	$tmp=explode('**',$GLOBALS["buff"]);
	$b1=$tmp[0];
	$bufs=explode(';',$tmp[1]);
	$res=Array();
	if(!empty($bufs)) foreach($bufs AS $b){
		$tmp=explode(':',$b);
		if($id3!=$tmp[1]) $res[]=$b;
	}
	$GLOBALS["buff"]=$b1.'**'.implode(';',$res);
	SetCookie('buff'/*.$table_id*/,$GLOBALS["buff"], time()+60*60*24*30,'/');
}

if(!empty($action) && $action=='buffer_clone' && /*!empty($GLOBALS["buff".$table_id]) &&*/ !empty($GLOBALS["buff"]) && strstr($GLOBALS["buff"],$table_id.'**') && $can_add){
	del_cache('rows',$table_id.'.'.$id2);
	//$aa=explode(';',$GLOBALS["buff".$table_id]);
	$tmp=explode('**',$GLOBALS["buff"]);
	$aa=explode(';',$tmp[1]);
	foreach($aa AS $var){
		$var=explode(':',$var);
		$id3=$var[1];
		$id4=$id6;//$var[0];
		//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
		seek_rlink($id3);
		if(!empty($rlink[$id3]) && check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
			clone_to($var[1],$id2,$table_id,$id,0,$id6);
			/*$sid=$id3;
			getrow($db,"SELECT * FROM main_row WHERE row_id=$sid",1,'main_row');
			$u=$db->Record["row_user"];
			if(empty($id5)){
				getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND owner_id=$id4 AND owner_table=$table_id AND owner_module=$id",1,'row_owner');
				if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
				$db->query("INSERT INTO row_owner (ro_pos, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
						VALUES ($pos, $id2, $id6, $sid, $id, $table_id, $id4, $table_id, $id, ".$u.")",3,'row_owner');
			} else {
				getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND owner_id=$id5 AND owner_table=$up_table_id AND owner_module=$id",1,'row_owner');
				if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
				$db->query("INSERT INTO row_owner (ro_pos, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
						VALUES ($pos, $id2, $id6, $sid, $id, $table_id, $id5, $table_id, $id, ".$u.")",3,'row_owner');
			}*/
		}
	}
}

if(!empty($action) && $action=='buffer_clear' && !empty($GLOBALS["buff"/*.$table_id*/])){
		SetCookie('buff'/*.$table_id*/,'', time()+60*60*24*30,'/');
		$GLOBALS["buff"/*.$table_id*/]='';
		$_COOKIE["buff"]='';
		$cur_buff=Array();
}


//=======================
//  Копирование
//=======================
if(!empty($action) && !empty($id3) && $action=='copy'){
	//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
	seek_rlink($id3);
	if(check_row($id3,$ex_table2,$ex_ex2v,'view',$rlink[$id3]->user,$rlink[$id3]->users)){
		del_cache('rows',$table_id.'.'.$id2);
		if(!empty($id6)) del_cache('row',$id6);
		getrow($db,"SELECT * FROM row_owner WHERE ro_id=$id4",1,'row_owner');
		copy_row($id3,$id4,$db->Record["owner_id"]);
		unset($action);
	}
}

//=======================
//  Удаление файла
//=======================
if(!empty($act2) && !empty($id3) && $act2=='delfile' && !empty($file)){
	seek_rlink($id3);
	if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
		getrow($db,"SELECT * FROM row_value WHERE value_table=$table_id AND value_row=$id3 AND value_col=$file",1,"row_value");
		if(!empty($db->Record["value_value"]) && file_exists(DOCUMENT_ROOT.$db->Record["value_value"])){
			$dbval=$db->Record["value_value"];
			del_file($dbval);
			//getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_value='".$dbval."'",1,"row_value");
			//if($db->Record["cnt"]<=1) unlink(DOCUMENT_ROOT.$dbval);
		}
		$db->query("UPDATE row_value SET value_value='' WHERE value_row=$id3 AND value_col=$file",3,'row_value');
		update_row_state($id3);
	}
	unset($act2);
}

//=======================
//  Активация
//=======================
if(!empty($action) && !empty($id3) && $action=='activate'){
	//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
	seek_rlink($id3);
	del_cache('rows',$table_id.'.'.$id2);
	if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
		del_cache('row',$id3);
		//$db->query("UPDATE main_row SET row_enable=1 WHERE row_id=$id3",3,'main_row');
		//$db->query("UPDATE row_owner SET ro_enable=1 WHERE row_id=$id3",3,'row_owner');
		$rows=getall($db,"SELECT * FROM row_owner WHERE row_id=$id3",1,'row_owner');
		if(count($rows)==1){
			$db->query("UPDATE main_row SET row_enable=1 WHERE row_id=$id3",3,'main_row');
			$db->query("UPDATE row_owner SET ro_enable=1 WHERE ro_id=$id5",3,'row_owner');
		} else {
			$db->query("UPDATE row_owner SET ro_enable=1 WHERE ro_id=$id5",3,'row_owner');
			$ena=false;
			foreach($rows AS $row) if(!$row["ro_enable"] && $row["ro_id"]!=$id5){$ena=true; break;}
			if(!$ena) $db->query("UPDATE main_row SET row_enable=1 WHERE row_id=$id3",3,'main_row');
		}		
		unset($action);
	}
	flush_cache();
}

//=======================
//  Деактивация
//=======================
if(!empty($action) && !empty($id3) && $action=='deactivate'){
	//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
	seek_rlink($id3);
	del_cache('rows',$table_id.'.'.$id2);
	if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
		del_cache('row',$id3);
		$rows=getall($db,"SELECT * FROM row_owner WHERE row_id=$id3",1,'row_owner');
		//это надо вынести в случае, если я захочу сделать раздельную активацию строк
		if(count($rows)==1){
			$db->query("UPDATE main_row SET row_enable=0 WHERE row_id=$id3",3,'main_row');
			$db->query("UPDATE row_owner SET ro_enable=0 WHERE ro_id=$id5",3,'row_owner');
		} else {
			$db->query("UPDATE row_owner SET ro_enable=0 WHERE ro_id=$id5",3,'row_owner');
			$ena=false;
			foreach($rows AS $row) if($row["ro_enable"] && $row["ro_id"]!=$id5){$ena=true; break;}
			if(!$ena) $db->query("UPDATE main_row SET row_enable=0 WHERE row_id=$id3",3,'main_row');
		}
		unset($action);
	}
	flush_cache();
}

//=======================
//  Выполнение части на строке
//=======================
if(!empty($action) && !empty($id3) && $action=='part_shell'){
	getrow($db,"SELECT * FROM main_part WHERE part_id=$part AND part_table=$table_id AND part_type=5",1,"main_part");
	if(!empty($db->Record["part_id"])){
		//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
		$tte=$ex_ex2/*v*/;
		seek_rlink($id3);
		if(check_row($id3,$ex_table2,$tte,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
			del_cache('row',$id3);
			echo shell_part($part,$tte,$id3);
			update_row_state($id3);
			unset($action);
		}
	}
}

//=======================
//  Смена пользователя
//=======================
if(!empty($action) && !empty($id3) && $action=='new_user' && isset($new_user)){
	//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
	seek_rlink($id3);
	if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
		$db->query("UPDATE main_row SET row_user='.$new_user.' WHERE row_id=$id3",3,"main_row");
		$db->query("UPDATE row_owner SET ro_user='.$new_user.' WHERE row_id=$id3",3,"row_owner");
		unset($action);
	}
}

//=======================
//  Добавление потомка
//=======================
if(!empty($action) && !empty($id3) && $action=='add_parent'){
//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
seek_rlink($id3);
if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users)){
	del_cache('rows',$table_id.'.'.$id2);
	$child=explode(',',substr(seek_double($id3),1));
	if(in_array($id4,$child)) echo '<div style="padding: 40px; font-size: 14px; color: #FF0000;">Невозможно клонировать элемент (ошибка рекурсивного зацикливания)</div>';
	else {
		if(!empty($id5)) $id6=$id5;
		clone_to($id3,$id2,$table_id,$id,$id4,$id6);
		/*$sid=$id3;
		getrow($db,"SELECT * FROM main_row WHERE row_id=$sid",1,'main_row');
		$u=$db->Record["row_user"];
		if(empty($id5)){
			getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND owner_id=$id4 AND owner_table=$table_id AND owner_module=$id",1,'row_owner');
			if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
			$db->query("INSERT INTO row_owner (ro_pos, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
					VALUES ($pos, $id2, $id6, $sid, $id, $table_id, $id4, $table_id, $id, ".$u.")",3,'row_owner');
		} else {
			getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND owner_id=$id5 AND owner_table=$up_table_id AND owner_module=$id",1,'row_owner');
			if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
			$db->query("INSERT INTO row_owner (ro_pos, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
					VALUES ($pos, $id2, $id6, $sid, $id, $table_id, $id5, $table_id, $id, ".$u.")",3,'row_owner');
		}*/
	}
	unset($action);
}
}

//=======================
//  Удаление - подтверждение
//=======================
if(!empty($action) && !empty($id3) && $action=='del'){	
	getrow($db,"SELECT * FROM row_owner WHERE row_id=$id3",1,"row_owner");
	if(!empty($db->Record)){ $tbl=$db->Record["row_table"]; $row=$db->Record["row_id"];}
	if(!empty($tbl)) getrow($db,"SELECT * FROM main_table WHERE table_id=$tbl",1,"main_table");
	if(!empty($db->Record)) $mjr=$db->Record["major_col"];
	if(!empty($db->Record)) getrow($db,"SELECT * FROM main_col WHERE col_id=$mjr",1,"main_col");
	if(!empty($db->Record)) $ct=$db->Record["col_type"];
	if(!empty($mjr)) getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=$mjr",1,"row_value");
	$name='';
	if(!empty($db->Record)){
		$v=$db->Record["value_value"];
		if($ct==0 || $ct==2 || $ct==3) $name=' "'.$v.'"';
		if($ct==1){
			if(!empty($_GET["title"])) $name="<br>Связь с элементами: ".$_GET["title"];
		}
		if($ct==4){
			if(strstr($v,':')){			
				$e=explode(':',$v);
				getrow($db,"SELECT * FROM main_module WHERE module_id=$e[0]",1,"main_module");
				$name='<br>Связь с модулем "'.$db->Record["module_name"];
				getrow($db,"SELECT * FROM ex_module WHERE ex_id=$e[1]",1,"ex_module");
				$name.=' - '.$db->Record["ex_name"];
				if(!empty($e[2])){
					getrow($db,"SELECT * FROM main_part WHERE part_id=$e[2]",1,"main_part");
					$name.=' - '.$db->Record["part_name"];
				}
				$name.='"';
			} else $name='Связь с модулем не установлена';
		}
	}
	$sort=0;
	if(isset($_GET["sort"])) $sort=$_GET["sort"];
	if(isset($_POST["sort"])) $sort=$_POST["sort"];
	
	echo '<div style="padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить строку'.$name.'?';
	echo '<form method="post" action="mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id4='.$id4.'&id7='.$id7.'&id3='.$id3.'&sort='.$sort.'&action=del_confirm">'.$df2;
	
	echo get_form_protection_key('mod_table',1,1);

	$rows=getall($db,"SELECT * FROM row_owner WHERE row_id=$id3",1,'row_owner');

	if(count($rows)>1){
		echo '<div><br>Есть несколько экземпляров этой строки. <br> <input type="radio" name="del" value="1" id="del1" style="width: auto" checked> <span class="link" onclick="document.getElementById(\'del1\').checked=true;">Удалить только этот экземпляр</span><br><input type="radio" name="del" value="2" id="del2" style="width: auto"> <span onclick="document.getElementById(\'del2\').checked=true;" class="link">Удалить все экземпляры</span></div>';	
	} else echo '<input type="hidden" name="del" value="2">';
	
	$usr_cols=getall3($db,"SELECT * FROM main_col WHERE col_table=$table_id AND col_type=5","col_id");
	$usr2=Array();
	if(!empty($usr_cols)){
		$usr=getall3($db,"SELECT * FROM row_value WHERE value_col IN (".implode(',',$usr_cols).") AND value_row=$id3","value_value",Array(),true);
		if(!empty($usr) && implode(',',$usr)!=''){
			$allcols=getall3($db,"SELECT * FROM main_col WHERE col_type=5","col_id");
			$except=getall3($db,"SELECT * FROM row_value WHERE value_col IN (".implode(',',$allcols).") AND value_row!=$id3 AND value_value IN (".implode(',',$usr).")","value_value");
			foreach($usr AS $u) if(empty($except[$u])) $usr2[$u]=$u;
		}
	}
	if(!empty($usr2)){
		$tmp_u=getall4($db,"SELECT * FROM main_auth WHERE auth_id IN (".implode(',',$usr2).")","auth_id");
		$usr3=Array();
		foreach($usr2 AS $usr) if(check_user(-$usr,'del') && isset($tmp_u[$usr])) $usr3[$usr]=$usr;
		if(!empty($usr3)){		
			echo '<div><br>Желаете ли вы удалить пользователей,<br>привязанных к этой строке?<br>';
			foreach($usr3 AS $usr) echo '<br><input type="checkbox" class="checkbox" name="ar['.$usr.']"> '.$tmp_u[$usr]["user_name"].' ('.$tmp_u[$usr]["user_login"].')';
			echo '</div>';
		}
	}
	
	echo '<input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">';
	echo '</form></div><br>';
	unset($action);
}

//=======================
//  Удаление - удаление
//=======================
if(!empty($action) && !empty($id3) && $action=='del_confirm' && (!empty($smb1) && $smb1=='Да') && check_form_protection_key($_POST['key'],'mod_table',1)){
	$rows=getall($db,"SELECT * FROM row_owner WHERE row_id=$id3",1,'row_owner');
	if(count($rows)>1 && $del==1){
		seek_rlink($id3);
		global $rlink;
		if(check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users))
			$db->query("DELETE FROM row_owner WHERE ro_id=$id4",1,'row_owner');
		unset($action);
	} else {
		//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
		seek_rlink($id3);
		global $rlink;
		if(isset($rlink[$id3]) && !empty($rlink[$id3]->rid) && check_row($id3,$ex_table2,$ex_ex2v,'del',$rlink[$id3]->user,$rlink[$id3]->users)) del_row($id3);
	}
	if(!empty($ar)) foreach($ar AS $auth_id=>$value)if(!empty($value) && check_user(-$auth_id,'del')) del_group($auth_id);
	unset($action);
}

//==========================
//  Импорт
//==========================
if(!empty($action) && $action=='import'){
	include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
	start_export();
	text_to_rows(file_get_contents($_FILES["data"]["tmp_name"]),$id2,$id6,0,$id7);
	end_export();
}

//=======================
//  Добавление / Изменение
//=======================
if(!empty($action) && ($action=='add_row' || $action=='edit_row')){
	$r=backup_globals();

	/*if($action=='edit_row') seek_rlink($id3);//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
	if($action!='edit_row' && !empty($id6)) seek_rlilnk($id6);//getrow($db,"SELECT * FROM main_row WHERE row_id=$id6");
	
	if(($action!='edit_row' && empty($id6) && check_row(0,$ex_table2,$ex_ex2v,'add'))
	||($action!='edit_row' && !empty($id6) && check_row($id6,$rlink[$id6]->table,$ex_ex2v,'edit',$rlink[$id6]->user,$rlink[$id6]->users))
	||($action=='edit_row' && check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users))){*/
	
	if(!isset($id3)) $id3=0;
	if(($action=='edit_form' && check_operation('edit',$id3,$id6,$ex_ex2v,$ex_table2)) || ($action!='edit_form' && check_operation('add',$id3,$id6,$ex_ex2v,$ex_table2))){

	if(!empty($id3)) del_cache('row',$id3);
	if($action=='add_row') $GLOBALS["f_action"]='add';
	else  $GLOBALS["f_action"]='edit';
	$GLOBALS["cur_ex"]=$ex_ex2;

	$ro_owner3=$ro_owner;$table_id3=$table_id;
	if(!empty($id6) && empty($ro_owner)){
		$ro_owner3=$id6;
		$table_id3=$up_table_id;
	}

	if($action=='add_row'){
		//Поменял тут? Не забудь поменять в func.inc add_row!
		del_cache('rows',$table_id.'.'.$id2);
		if(empty($id6)) $row_sub=0; else {
			$row_sub=$id6;
			del_cache('row',$row_sub);
		}
		if(!empty($ro_owner3)){		
			del_cache('row',$ro_owner3);
		}
		$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user, row_uin, modified_date, creation_date)
				VALUES ($id, $table_id, $id2, $row_sub, ".$user->id.",'".uuin()."', '".date('Y-m-d H:i:s')."', '".date('Y-m-d H:i:s')."')",3,'main_row');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
		$ro_sub=0;if(!empty($id6)) $ro_sub=$id6;
		getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND row_table=$table_id AND owner_id=$ro_owner3 AND owner_table=$table_id3 AND ro_sub=$ro_sub AND owner_module=$id",1,'row_owner');
		if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
		if(empty($id6)) $ro_sub=0; else $ro_sub=$id6;
		$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, ro_sub, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
				VALUES ($pos, $id2, $sid, $ro_sub, $id, $table_id, $ro_owner3, $table_id3, $id, ".$user->id.")",3,'row_owner');
	} else {
		$backup=backup_row($id3);
		$GLOBALS["backrow"]=$backup;
		$sid=$id3;
		if(!empty($id6) && !empty($ro_owner2) && $ro_owner2!=$id6){
			getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND row_table=$table_id AND owner_id=$ro_owner2 AND owner_module=$id",1,'row_owner');
			if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
			getrow($db,"SELECT * FROM main_row WHERE row_id=$ro_owner2",1,"main_row");
			$ntable=$db->Record["row_table"];			
			$db->query("UPDATE row_owner SET owner_id=$ro_owner2, ro_pos=$pos, owner_table=$ntable WHERE ro_id=$id5",3,'row_owner');
		} else {
			getrow($db,"SELECT * FROM row_owner WHERE ro_id=$id5",1,'row_owner');
			if($db->Record["owner_id"]!=$ro_owner3){
				$ro_sub=0;if(!empty($id6)) $ro_sub=$id6;
				getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex=$id2 AND row_table=$table_id AND owner_id=$ro_owner3 AND owner_table=$table_id3 AND ro_sub=$ro_sub AND owner_module=$id",1,'row_owner');
				if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
				getrow($db,"SELECT * FROM main_row WHERE row_id=$ro_owner3",1,"main_row");
				$ntable=$db->Record["row_table"];
				if(empty($ntable)) $ntable='0';
				$db->query("UPDATE row_owner SET owner_id=$ro_owner3, ro_pos=$pos, owner_table=$ntable WHERE ro_id=$id5",3,'row_owner');
			}
		}
		update_row_state($id3);
		del_vals_pre($id3,$table_id);
	}

	$GLOBALS["cancel"]=false;
	echo insert_values($table_id,$id,$sid);
	if($GLOBALS["cancel"] && $action=='add_row'){
		del_row($sid);
		if($GLOBALS["cancel"]!=-1){
			if($GLOBALS["cancel"]==1) echo '<div align="center"><h2 style="color:#FF0000;">Добавление не удалось</h2></div>';
			else echo '<div align="center"><h2 style="color:#FF0000;">Добавление не удалось по причине: '.$GLOBALS["cancel"].'</h2></div>';
		}
	}
	if($GLOBALS["cancel"] && $action!='add_row'){
		del_vals(" value_table!=0 AND value_row=$sid");
		copy_vars($backup,$sid);
		if($GLOBALS["cancel"]!=-1){
			if($GLOBALS["cancel"]==1) echo '<div align="center"><h2 style="color:#FF0000;">Изменение не удалось</h2></div>';
			else echo '<div align="center"><h2 style="color:#FF0000;">Изменение не удалось по причине: '.$GLOBALS["cancel"].'</h2></div>';
		}
		del_row($backup,1);
	}	
	if(!$GLOBALS["cancel"] && $action!='add_row'){
		del_row($backup,1);
	}
	$GLOBALS["backrow"]=0;
	
	//Добавляем индивидуальные настройки для подтаблиц
	if(!$GLOBALS["cancel"]){
		$all_subtables=get_table_subtables_m($table_id);
		if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value)if(/*$var!=$id && */!empty($_POST['subtable'.$var])){
			echo insert_values($table_id,$var,$sid,0,'',1,1,1,1);
		}
	}

	unset($id3);
	$GLOBALS["f_action"]='';
	$action='';
	}
	return_globals($r);
	flush_cache();
}

//==========================
//  Вывод строк
//==========================

$GLOBALS["cur_ex"]=$ex_ex2;
$GLOBALS["cur_module"]=$cum;//$module_id;
$GLOBALS["cur_table"]=$table_id;

	// вывод сопутствующих таблиц

/*if(!empty($GLOBALS["cex".$id]))$ccx=$GLOBALS["cex".$id]; else */$ccx=$ex_ex2;
if($use_crosstables){
	$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=".$ccx." AND ex_module=$id",1,"ex_group");
	$ex=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) $ex[$tm["ex_table"]]=$tm["ex_ex1"];
	$add1=" AND table_id!=$table_id";if(!empty($id6)) $add1='';
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold!=2".$add1,1,"main_table");
	if(!empty($tbl)){
		$first=true;
		$ci=0;
		foreach($tbl AS $tb)if(!empty($ex[$tb["table_id"]]) && check_tbl($tb["table_id"],'view')){
			$ci++;
			if($first){
				echo '<table id="records" cellpadding="3" cellspacing="1"><tr>';
				$first=false;
			}
			if($ci % 4 == 1) echo '</tr><tr>';
			echo '<td><a href="mod_table?id='.$id.'&amp;id2='.$ex[$tb["table_id"]].'">'.$tb["table_name"].'</a></td>';
		}
		if(!$first) echo '</tr></table>';
	}
}

reset_se();

	// подгрузка строк

function load_rows($rows,$cols,$ena=1){
	global $db,$row_ids,$vals2,$vals3,$vals4,$table_id,$ex_ex2,$ex_ex2v,$mjc,$ms,$tmj,$srow,$foption_child;
	
	if(empty($cols)) return Array();
	$in2=Array();
	foreach($rows AS $var=>$value)if($value!='hidden' && check_row($value->id,$table_id,$ex_ex2v,'view',$value->user,$value->users) && ($ena==1 || ($ena==2 && $value->enable) || ($ena==3 && !$value->enable))){
		$in2[]=$value->id;
	}
	$in=Array();
	if(!empty($cols)) foreach($cols AS $col) $in[]=$col["col_id"];
	if(!empty($in) && !empty($in2)) $tmp=getall($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$in2).") AND value_table!=0 AND value_col IN (".implode(',',$in).")",1,'row_value');
	//$crows=Array();
	if(!empty($tmp)) foreach($tmp AS $tm){
		$vals2[$tm["value_row"]][$tm["value_col"]]=$tm["value_value"];
		$vals3[$tm["value_row"]][$tm["value_col"]][]=$tm["value_value"];
		//$crows[$tm["value_row"]]=1;
	}
	/*$crows=implode(',',array_flip($crows));
	if(!empty($crows)){
		$tmp=getall($db,"SELECT * FROM row_owner WHERE row_id IN ($crows)",1,"main_row");
		rows_to_rlink($tmp);
	}*/
	
	foreach($rows AS $var=>$value)if($value!='hidden' && check_row($value->id,$table_id,$ex_ex2v,'view',$value->user,$value->users) && ($ena==1 || ($ena==2 && $value->enable) || ($ena==3 && !$value->enable))){
		$row_ids[$value->id][$value->rid]=1;
		//if(empty($vals3)){
		
			//$in=Array();
			//foreach($cols AS $col) $in[]=$col["col_id"];
		
			//эту строчку внизу нужно заменить
			//т.е. нужно грузить только значения для определённых строк, которые находятся в зоне видимости
			//из-за этого, в частности, если в таблице 1000 статей, то это приведёт к 1000 запросам
			//if(!empty($in)) $tmp=getall($db,"SELECT * FROM row_value WHERE value_table=$table_id AND value_col IN (".implode(',',$in).")",1,'row_value');
			
			/*последний рабочий вариант
			if(!empty($in)) $tmp=getall($db,"SELECT * FROM row_value WHERE value_row=$value->id AND value_table!=0 AND value_col IN (".implode(',',$in).")",1,'row_value');
			if(!empty($tmp)) foreach($tmp AS $tm){
				$vals2[$tm["value_row"]][$tm["value_col"]]=$tm["value_value"];
				$vals3[$tm["value_row"]][$tm["value_col"]][]=$tm["value_value"];
			}
			*/
			
			//echo count($vals3).'<br>';
		//}
		if(!empty($vals2[$value->id])) $vals=$vals2[$value->id]; else $vals='';
		foreach($cols AS $col){
			if(!isset($vals[$col["col_id"]])) $vals[$col["col_id"]]='';
			$rows[$var]->val[$col["col_id"]]=$vals[$col["col_id"]];
			if($col["col_type"]==1){
				$rows[$var]->dst[$col["col_id"]]=$rows[$var]->val[$col["col_id"]];
				if($vals[$col["col_id"]]!=-1){
					if(!empty($vals3[$value->id][$col["col_id"]])){
						$trs2=$vals3[$value->id][$col["col_id"]];

						$srow2=Array();
						for($i=0;$i<count($trs2);$i++) if(empty($srow[$trs2[$i]]) && empty($srow2[$trs2[$i]])) $srow2[$trs2[$i]]=1;
						if(!empty($srow2)){
							$srow2s='';
							foreach($srow2 AS $var2=>$value2)if(is_numeric($var2)){ if($srow2s!='') $srow2s.=','; $srow2s.=$var2; }
							if(!empty($srow2s)){
								$srow2a=getall($db,"SELECT * FROM main_row WHERE row_id IN ($srow2s)",2,'main_row');
								if(!empty($srow2a)) foreach($srow2a AS $srow2e){
									$srow[$srow2e["row_id"]]=$srow2e;
								}
							}
						}
						$val='';
						for($i=0;$i<count($trs2);$i++) if(!empty($srow[$trs2[$i]])) {
							$r=$srow[$trs2[$i]];
							if(empty($tmj[$r["row_table"]])) $tmj[$r["row_table"]]=seek_major($r["row_table"]);
							$ccol=$tmj[$r["row_table"]]["col_id"];
							if(empty($vals4[$ccol]) && !empty($ccol)){
								$vals4[$ccol]=Array();
								$tmp=getall($db,"SELECT * FROM row_value WHERE value_col=".$ccol,1,'row_value');
								if(!empty($tmp)) foreach($tmp AS $tm) $vals4[$ccol][$tm["value_row"]]=$tm;
							}
							if(!empty($val)) $val.='<br>';
							if(!empty($vals4[$ccol][$trs2[$i]]["value_value"])) $val.=$vals4[$ccol][$trs2[$i]]["value_value"];
						}
						$rows[$var]->val[$col["col_id"]]=$val;
					}
				} else $rows[$var]->val[$col["col_id"]]='';
			}
			if($col["col_type"]==2){
				$rows[$var]->dst[$col["col_id"]]=$rows[$var]->val[$col["col_id"]];
				if($rows[$var]->val[$col["col_id"]]) $rows[$var]->val[$col["col_id"]]='да';
				else $rows[$var]->val[$col["col_id"]]='нет';
			}
			if($col["col_type"]==4){
				$rows[$var]->dst[$col["col_id"]]=$rows[$var]->val[$col["col_id"]];
				if(empty($vals[$col["col_id"]])) $rows[$var]->val[$col["col_id"]]='Нет'; else {
					$tmp=module_select(0,0,$rows[$var]->val[$col["col_id"]],1,3);
					if(empty($tmp)) $tmp='Нет';
					$rows[$var]->val[$col["col_id"]]=$tmp;
				}
			}
			if($col["col_type"]==5){
				$rows[$var]->dst[$col["col_id"]]=$rows[$var]->val[$col["col_id"]];
				if(empty($vals[$col["col_id"]])) $rows[$var]->val[$col["col_id"]]='Нет'; else {
					$tmp=user_select($col["col_link"],$rows[$var]->val[$col["col_id"]],1);
					if(empty($tmp)) $tmp='Нет';
					$rows[$var]->val[$col["col_id"]]=$tmp;
				}
			}
		}
		if(!empty($rows[$var]->sub) && !$foption_child) $rows[$var]->sub=load_rows($rows[$var]->sub,$cols,$ena);
	}
	return $rows;
}

	// сортировка строк (пока не реализовано) //upt реализовано ниже

//function sort_rows(&$rows){
//	if(!empty($rows->sub)) sort_rows($rows->sub);
//}

	// вывод строк

function show_rows($rows,$cols,$own=0,$step=0,$start=0,$end=0,$ena=1,$url=''){
	global $id,$id2,$crows,$crows_c,$theader,$rub_opt,$table_multy,$loaded,$tid,$row_ids,$tsubs,$tmj,$id6,$id7,$global_rows,$chk,$table_id,$ex_ex2,$ex_ex2v,$ex_ex1,$cum,$sort,$df,$df2,$df3,$base_root,$url_rows,$foption_child;
	$ci=0;
	$ri=0;
	
	$cols_count=3;//1 для номера и 1 для действий и 1 для неизвестного косяка
	foreach($cols AS $col) if(check_col($col["col_id"],'view')) $cols_count++;
	if(!empty($tsubs)) $cols_count++;//ещё 1 для подтаблиц
	
	if(empty($cols)) return Array();
	foreach($rows AS $row){
	//if(isset($row->rlink[$row->owner][$ri])) $row->rid=$row->rlink[$row->owner][$ri];//это мега заглушка от того, что невозможно определить rid, т.к. он будет одинаковый
	$ri++;
	$first_row=true;
	$second_row=true;
	global $tree_vars,$tv_cache;
	$tree_vars[$GLOBALS["spec_step2"]]=Array();
	
	if(!is_object($row) && $row=='hidden'){
		echo '<tr><td colspan="'.$cols_count.'">Вам не хватает прав доступа для просмотра информации о данном объекте</td></tr>';
	} else if(check_row($row->id,$table_id,$ex_ex2v,'view',$row->user,$row->users) && ($ena==1 || ($ena==2 && $row->enable) || ($ena==3 && !$row->enable))){
		if($foption_child){
			global $fowner_cache;
			if(!isset($fowner_cache[$row->owner])) $fowner_cache[$row->owner]=getrowval("SELECT * FROM row_owner WHERE row_id=".$row->owner,"ro_id");
			$own=$fowner_cache[$row->owner];
		}
		//if(isset($row->or[$own])) $row->rid=$row->or[$own];
		$can_edit=check_row($row->id,$table_id,$ex_ex2v,'edit',$row->user,$row->users);
		$can_double=check_row($row->id,$table_id,$ex_ex2v,'add',$row->user,$row->users);
		$can_del=check_row($row->id,$table_id,$ex_ex2v,'del',$row->user,$row->users);
		if(!empty($GLOBALS["start_url2"]) && isset($url_rows[$row->id]) && $url_rows[$row->id]!='-') $GLOBALS["admin_row_url"]=$GLOBALS["start_url2"].$url.$url_rows[$row->id]; else $GLOBALS["admin_row_url"]='';
		if($end!=0){
			$ci++;
			if(($ci-1)<$start || ($ci-1)>$end) continue;
		}
		if(empty($tid)) $tid=1;else $tid++;
		if(!empty($loaded[$row->id])) $fastedit_enable=0;
		else $fastedit_enable=1;
		$loaded[$row->id]=1;
		if(!$can_edit) $fastedit_enable=0;
		$adc=Array();
		if(!$row->enable) $adc[]='na';
		if(!$can_edit) $adc[]='noDrag';
		if(!empty($row->sub)) $adc[]='do_next';
		if(!empty($adc)) $adc=' class="'.implode(' ',$adc).'"'; else $adc='';
		echo '<tr id="r'.$row->rid.':'.$row->id.'"'.$adc.'>';
		
		/*if(empty($row->sub))
		else echo '<tr ondblClick="showhide(\'rows'.$row->rid.':'.$row->id.'\');"'.$adc.'>';*/		
		/*if(!empty($row->sub)) echo '<td width="45" style="cursor: pointer; background-color: #D9E3EA;" OnClick="showhide(\'rows'.$tid.'\');">';
		else */
		$chk['chk['.$row->rid.':'.$row->id.']']=$row->rid.':'.$row->id;
		if($can_edit){
			$ac='';
			if($can_edit) $ac=' class="drag2"';
			echo '<td width="5" '.$ac.'>';
		
			echo se('move','','','',0,0);
			echo '</td>';
			$rp=0;
			if(strlen($row->pos)>2) $rp=(strlen($row->pos)-2)*9;
			if(!empty($row->sub)) echo '<td width="'.(62+$rp).'"><nobr>';
			else echo '<td width="'.(41+$rp).'"><nobr>';
			echo ' <input name="pos['.$row->rid.']" class="pos" id="e'.$row->rid.':'.$row->id.'" type="text"';
			if(strlen($row->pos)>2)  echo ' style="width: '.(9*strlen($row->pos)).'px;"';
			echo ' value="'.$row->pos.'" OnFocus="show(\'chpanel\');">';
			if($step!=0) $postfix='k'; else $postfix='';
			echo '<![if !IE]><input type="checkbox" class="checkbox1'.$postfix.'" id="'.$row->rid.':'.$row->id.'" name="chk['.$row->rid.':'.$row->id.']" OnClick="show(\'chpanel\');show(\'chpanel2\');show(\'buff\');selr(\''.$row->rid.':'.$row->id.'\');"><![endif]><!--[if IE]><input type="checkbox" class="checkbox2'.$postfix.'" id="'.$row->rid.':'.$row->id.'" name="chk['.$row->rid.':'.$row->id.']" style="float: right;" OnClick="show(\'chpanel\');show(\'chpanel2\');show(\'buff\');selr(\''.$row->rid.':'.$row->id.'\');"><![endif]-->';
		} else {
			//echo '<td width="54" colspan="2"><nobr>';
			echo '<td colspan="2"><nobr>'.$row->pos;
		}
		if(!empty($row->sub) && !$foption_child) echo se('tree','','',' class="link" id="tree'.$tid.'" OnClick="flipimg(\'tree'.$tid.'\',\''.$base_root.'/files/editor/tree.png\',\''.$base_root.'/files/editor/tree2.png\'); showhide(\'rows'.$row->rid.':'.$row->id.'\');"',0,2);
		echo '</nobr></td>';		
		foreach($cols AS $col)if(check_col($col["col_id"],'view')){
			$fastedit_enable2=$fastedit_enable;
			if($fastedit_enable2 && !check_col($col["col_id"],'edit')) $fastedit_enable2=false;
			echo '<td>';
			$val=$row->val[$col["col_id"]];
			if($col["col_fastedit"] && $can_edit && $fastedit_enable2 && $col["col_type"]==0) echo '<input type="text" name="fe['.$row->id.']['.$col["col_id"].']" style="margin: 0px; padding: 0px;" value="'.htmlspecialchars($val,3).'" OnFocus="show(\'chpanel\');">';
			else if($col["col_onshow"]!=''){
				global $f_type;
				$oldft=$f_type;
				$f_type='onshow';
				$r=backup_globals();
				$GLOBALS["cur_row"]=$row->id;
				//$GLOBALS["cur_col"]=$col["col_sname"];
				unset($GLOBALS["cur_col"]);
				$GLOBALS["cur_col"]->id=$col["col_id"];
				$GLOBALS["cur_col"]->row=$row->id;
				global $ex_module,$module_id,$table_id,$ex_ex2;
				$GLOBALS["cur_ex"]=$ex_ex2;//$ex_module;
				$GLOBALS["cur_module"]=$cum;//$module_id;
				$GLOBALS["cur_table"]=$table_id;
				$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];
				echo shell_tpl($col["col_onshow"]);
				$GLOBALS["exit"]=false;
				$GLOBALS["break"]=false;
				$GLOBALS["continue"]=false;
				$GLOBALS['break']=false;
				$GLOBALS['xbreak']=false;
				
				if($GLOBALS["firstt"]){
					if(tree_optimize($row,$rows)) $GLOBALS["firstt"]=false;
				}				
				
				return_globals($r);
				$f_type=$oldft;
			} else {
				if($col["col_fastedit"] && $fastedit_enable2 && $col["col_type"]==1 && $col["col_link2"]==0){				
					echo '<select name="fe['.$row->id.']['.$col["col_id"].']" style="padding: 0px; margin: 0px;" OnChange="show(\'chpanel\');"><option value="0">Выберите значение</option>';
					
					/*$col_deep=$col["col_deep"];
					$col_link3=$col["col_link3"];
					if(!empty($col_deep) && $col_link3==3){
						if(strstr($col_deep,'.')){
							$tmp=explode('.',$col_deep);
							$ntbl=$tmp[count($tmp)-1];
						} else $ntbl=$col_deep;
					} else $ntbl=0;					
					$level=seek_max_level($crows[$col["col_id"]]);
					if($col_link3!=3) $level=0;
					options($crows[$col["col_id"]],' ',$row->dst[$col["col_id"]],1,1,0,0,'',Array(),10000,0,$col["col_link3"],$level,$ntbl);*/
					
					global $spc_val;
					$crval=$row->dst[$col["col_id"]];		
					if(!empty($crval) && empty($crows_c[$col["col_id"]][$crval])){
						if(!isset($spc_val[$crval])){
							$spc_val[$crval]=get_basename($crval);
						}
						echo '<option value="'.$crval.'" selected>'.$spc_val[$crval].'</option>';
					}
					options($crows[$col["col_id"]],' ',$row->dst[$col["col_id"]]);
					
					echo '</select>';
				}
				else if($col["col_fastedit"] && $fastedit_enable2 && $col["col_type"]==2){
					if($row->dst[$col["col_id"]]==1) $add=' checked'; else $add='';
					echo '<input name="fe['.$row->id.']['.$col["col_id"].']" type="checkbox" OnChange="show(\'chpanel\');" class="button"'.$add.'>';
					echo '<input name="bfe['.$row->id.']['.$col["col_id"].']" type="hidden" value="1">';
				}
				else if($col["col_type"]==3 && !empty($val) && file_exists(DOCUMENT_ROOT.$val)){
					echo '<a href="'.$val.'">Загрузить</a> (размер: '.smart_size(filesize(DOCUMENT_ROOT.$val)).')';
				}
				else if($col["col_fastedit"] && $fastedit_enable2 && $col["col_type"]==4){
					echo '<select name="fe['.$row->id.']['.$col["col_id"].']" style="padding: 0px; margin: 0px;" OnChange="show(\'chpanel\');">';
					echo module_select($col["module_url"],$col["module_type"],$row->dst[$col["col_id"]],0,0,2);
					echo '</select>';
				}
				else if($col["col_fastedit"] && $fastedit_enable2 && $col["col_type"]==5){
					echo '<select name="fe['.$row->id.']['.$col["col_id"].']" style="padding: 0px; margin: 0px;" OnChange="show(\'chpanel\');">';
					echo '<option value="0">Выберите пользователя</option>';
					echo user_select($col["col_link"],$row->dst[$col["col_id"]]);
					echo '</select>';
				}
				else if((!$col["col_fastedit"] || !$fastedit_enable) && $col["col_type"]==4){
					if(empty($row->dst[$col["col_id"]]) || !strstr($row->dst[$col["col_id"]],':')) echo 'нет';
					else {
						$mo=explode(':',$row->dst[$col["col_id"]]);
						global $db;
						if(empty($mo[0])) echo $val;
						else getrow($db,"SELECT * FROM main_table WHERE table_module=$mo[0] AND table_bold=1",1,"main_table");
						if(empty($db->Record)) echo $val;
						else if(!empty($mo[0])) {
							$tbl=$db->Record["table_id"];
							$tex=0;
							if(!empty($mo[2])){
								$ntbl=getrowval("SELECT part_id, part_table FROM main_part WHERE part_id=".$mo[2],"part_table");								
								if(!empty($ntbl)) $tbl=$ntbl;
								else {
									$ntbl=getrowval("SELECT part_id, part_owner, part_table FROM main_part WHERE part_owner=".$mo[2]." AND part_table!=0","part_table");
									if(!empty($ntbl)) $tbl=$ntbl;
								}
							}
							getrow($db,"SELECT * FROM ex_group WHERE ex_module=$mo[0] AND ex_table=$tbl AND ex_ex2=$mo[1]",1,"ex_group");
							$tex=$db->Record["ex_ex1"];
							if(!empty($db->Record["ex_ex2"])){
								getrow($db,"SELECT * FROM ex_module WHERE ex_id=".$db->Record["ex_ex2"],1,"ex_module");
								if($db->Record["ex_major"]!=0){
										getrow($db,"SELECT * FROM ex_group WHERE ex_module=$mo[0] AND ex_table=".$db->Record["ex_major"]." AND ex_ex2=$mo[1]",1,"ex_group");
										$tex=$db->Record["ex_ex1"];
								}
							}
							if(empty($tex)) echo $val;
							else {
								echo '<a href="mod_table?id='.$mo[0].'&amp;id2='.$tex.'">'.$val.'</a>';
							}
						}
					}
					//echo module_select($col["module_url"],$col["module_type"],$row->dst[$col["col_id"]],0,0,2);
				}
				else {
					if(!empty($tmj[$col["col_table"]]) && $col["col_id"]==$tmj[$col["col_table"]]["col_id"] && !empty($row->sub)){
						echo '<span class="link" OnClick="flipimg(\'tree'.$tid.'\',\''.$base_root.'/files/editor/tree.png\',\''.$base_root.'/files/editor/tree2.png\'); showhide(\'rows'.$row->rid.':'.$row->id.'\');">'.$val.'</span>';
					} else echo $val;
				}
			}
			echo '</td>';
		}
		if(!empty($tsubs)){
			echo '<td>';
			/*if($can_edit)*/foreach($tsubs AS $t_id=>$t_name){
				
				/*$z=get_sub($row->id,$t_id,0,1,0,0,0,$ex_ex1,$ex_ex2,$t_id,0,$t_id,1);//если сюда вставить проверку аутентификации, то у зарегистрированного пользователя будет очень много запросов (из-за проверки глубинного родителя в check_row)
				//так что информация о включённых строках может быть неточна, юзер может видеть также кол-во строк ему не принадлежащих
				$e=0;$d=0;
				if(!empty($z)){
					foreach($z AS $zc) if($zc->enable) $e++; else $d++;//сюда незабыть вставить проверку прав на эти строки. а то получится некрасиво - вижу что 10 строк, захожу - а там фига :)
					$add='&nbsp;(';
					if(!empty($e)) $add.='<span style="color:#00AA00;">'.$e.'</span>';
					if(!empty($e) && !empty($d)) $add.='/';
					if(!empty($d)) $add.='<span style="color:#AA0000;">'.$d.'</span>';
					$add.=')';
				} else $add='';*/
				
				$add=get_table_chields_count($row->id,$t_id);
				
				echo '<a href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$row->id.'&amp;id7='.$t_id.'">'.$t_name.'</a>'.$add.'<br>';
			}
			echo '</td>';
		}
		echo '<td width="50"><nobr>';
		global $spec_act;
		if(!empty($spec_act) && $can_edit) foreach($spec_act AS $sa){
			if($sa->if=='' || prep_do_if($sa->if,$sa->self,$row->id,$ex_ex2v)){
				if($sa->pic=='') echo '<a href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=part_shell&amp;part='.$sa->id.'&amp;sort='.$sort.'">'.$sa->name.'</a> ';
				else echo se('/files/editor/icons/'.$sa->pic,'mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=part_shell&amp;part='.$sa->id.'&amp;sort='.$sort,$sa->name);
			}
		}
		global $cur_buff;
		if(isset($cur_buff[$row->id])){
			echo se('buffer_del','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id5='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=buffer_del&amp;sort='.$sort);
		}
		if($can_edit){
			if($row->enable) echo se('deactivate','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id5='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=deactivate&amp;sort='.$sort);
			else echo se('activate','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id5='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=activate&amp;sort='.$sort);
		}
		if($can_double) echo se('copy','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id4='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=copy&amp;sort='.$sort); 
		if($table_multy && $can_edit){
			echo se('link','','','class="link" OnClick="java_sub('.$tid.','.$row->id.');"',0);
		}
		if($can_edit) echo se('edit','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id4='.$own.'&amp;id5='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=edit_form&amp;sort='.$sort.'#editform');
		$nu='mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.$df.'&action=new_user&id3='.$row->id.'&id4='.$own.'&id5='.$row->rid.'&sort='.$sort;
	
		$uname='user';	
		if($can_edit){
			$sv='';
			if(!empty($row->users)){
				$uname='usrs';
				$sv=' ('.count($row->users).')';	
			}
			$f="click_user($tid,'$nu',$row->user,$row->id,'$sv',dq);";
			//echo se($uname,'','',' class="link" OnClick="showhide(\'udiv'.$tid.'\'); replace_content(\'usel\',\'u2div'.$tid.'\',\'%url%\',\''.$nu.'\',dq+\''.$row->user.'\'+dq,dq+\''.$row->user.'\'+dq+\' selected\',\'%id%\',\''.$row->id.'\',\'%sv%\',\''.$sv.'\');"',0);
			echo se($uname,'','',' class="link" OnClick="'.$f.'"',0);
		} else {
			echo se($uname,'','',' class="link" OnClick="showhide(\'udiv'.$tid.'\')"',0);
		}
		$tit='';
		if(!empty($col) && $col["col_type"]==1) $tit='&amp;title='.urlencode($val);
		$xurl='';
		if(isset($url_rows[$row->id]) && !empty($GLOBALS["start_url2"]) && $url_rows[$row->id]!='-'){
			echo se('anchor',$GLOBALS["start_url2"].$url.$url_rows[$row->id]);
			$xurl=$url.$url_rows[$row->id];
			if(!empty($xurl) && $xurl[strlen($xurl)-1]!='/') $xurl.='/';
		}
		if($can_del) echo se('del','mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$row->id.'&amp;id4='.$row->rid.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'&amp;action=del&amp;sort='.$sort.$tit);

		//пользователь
		echo '<div id="udiv'.$tid.'" style="display: none;">';
		if(check_user(-$row->user,'view')){
			global $ucache,$db;
			if($row->user>0){
				if(!isset($ucache[$row->user])){
					getrow($db,"SELECT * FROM main_auth WHERE auth_id=$row->user",1,"main_auth");
					$ucache[$row->user]=$db->Record;
				}
				$u=$ucache[$row->user];
			}
			echo '<div style="margin-top: 10px; margin-bottom: 5px;" align="center">'.si('user');
			if($row->user==0){
				echo '<b>Гость</b>';
				//echo '<b>Суперпользователь</b>';
			} else if($row->user==-1){
				echo '<b>Суперпользователь</b>';
				//echo '<b>Гость</b>';
				echo se('mail','mail?to='.$row->user,'');
			} else {
				echo '<b>'.$u["user_name"].'</b> ('.$u["user_login"].') ';
				echo se('mail','mail?to='.$row->user,'');
			}
			echo '<div id="u2div'.$tid.'"></div>';
			echo '</div>';
		} else {
			global $user;
			echo '<div style="margin-top: 10px; margin-bottom: 5px;" align="center">'.si('user');
			if($row->user!=$user->id) echo '<b>Скрытый</b>'; else echo '<b>Текущий</b>';
			echo '<div id="u2div'.$tid.'"></div>';
			echo '</div>';
		}
		echo '</div>';

		//связь - клонирование
		if($table_multy && $can_edit){
			echo '<div id="pan'.$tid.'" style="display: none;">';
			echo '</div>';
		}

		echo '</nobr></td>';
		echo '</tr>';
		
		//if($second_row){
		if($first_row){
			tree_optimize($row,$rows);//спорное решение. уменьшает кол-во запросов к БД, но увеличивает скорость загрузки админки
		}		
		
		if(!empty($row->sub) && !$foption_child){
			echo '<tr';
			echo ' class="noDrag noDrop" style="display: none;"';
			echo ' style="display: none;"';
			$cspan=3;
			if(!empty($tsubs)) $cspan++;
			echo ' id="rows'.$row->rid.':'.$row->id.'"><td colspan="'.(count($cols)+$cspan).'">';
			echo '<script>$(document).ready(function() { $(".rcrds'.$tid.'").tableDnD({      onDragClass: "drag1",      dragHandle: "drag2" });  });</script>';
			echo '<table id="records" cellpadding="3" cellspacing="1" class="rcrds'.$tid.'">';
			//echo $theader;
			show_rows($row->sub,$cols,$row->rid,$step+1,0,0,$ena,$xurl);
			echo '</table>';
			echo '</td></tr>';
		}

		//if($second_row) $second_row=false;
		//if($first_row){
		//	$second_row=true;
		//	$tree_vars[$GLOBALS["spec_step2"]]=Array();
		//}
		$first_row=false;	
	
	}}
}

		$ena=1;//1 - all, 2 - enable, 3 - disable
		if(!empty($action) && $action=='show_all'){
			SetCookie('show'.$id2.$id6,1, time()+60*60*24*30,'/');
			$_COOKIE["show".$id2.$id6]=1;
		}
		if(!empty($action) && $action=='show_enable'){
			SetCookie('show'.$id2.$id6,2, time()+60*60*24*30,'/');
			$_COOKIE["show".$id2.$id6]=2;
		}
		if(!empty($action) && $action=='show_disable'){
			SetCookie('show'.$id2.$id6,3, time()+60*60*24*30,'/');
			$_COOKIE["show".$id2.$id6]=3;
		}
		if(empty($_COOKIE["show".$id2.$id6])) $ena=1; else $ena=$_COOKIE["show".$id2.$id6];

$GLOBALS["f_type"]='top';
if(!empty($table_top)){
	echo shell_tpl($table_top);
	$GLOBALS["exit"]=false;
	$GLOBALS["break"]=false;
	$GLOBALS["continue"]=false;
	$GLOBALS['break']=false;
	$GLOBALS['xbreak']=false;	
	flush_cache();
}
$GLOBALS["f_type"]='';

$exes=get_exes($id6,$ex_ex1);

$query='SELECT SUM(CASE WHEN ro_enable=1 THEN 1 ELSE 0 END) AS ena, SUM(CASE WHEN ro_enable=1 THEN 0 ELSE 1 END) AS disa, count(ro_id) AS total FROM row_owner WHERE ro_ex IN ('.$exes.') AND owner_id='.$id6;
getrow($db,$query,1,"row_owner");
$disabled=$db->Record["disa"];
$enabled=$db->Record["ena"];
$total=$db->Record["total"];

if($ena>1){
	if($disabled==0) $ena=1;
	if($enabled==0) $ena=1;	
}
$SQL_enable='';
if($ena==2) $SQL_enable=' AND ro.ro_enable=1';
if($ena==3) $SQL_enable=' AND ro.ro_enable=0';

$SQL_leftjoin='';
$SQL_where='';

$f_id=0;

$foption_child=(!empty($f) && is_array($f) && !empty($f["option_child"]));
if(!empty($f) && is_array($f)) foreach($f AS $col_id=>$value)if(!empty($value) && isset($fcols[$col_id])){
	$f_id++;
	$change=false;
	$col=$fcols[$col_id];
	$SQL_leftjoin.=" LEFT JOIN row_value AS rv".$f_id." ON rv".$f_id.".value_row=ro.row_id AND rv".$f_id.".value_col=".$col["col_id"];
	if($col["col_filter"]==1){
		if($col["col_type"]==0){
			//текст
			$cv="rv".$f_id.".value_value";
			if($uc[$col_id]==2){
				$value=strtolower($value);
				$cv="LOWER(rv".$f_id.".value_value)";
			}
			if($wh[$col_id]==1) $value=" LIKE '%".$value."%'"; else if($wh[$col_id]==2) $value="='".$value."'";
			else $value=" NOT LIKE '%".$value."%'";
			$SQL_where.=" AND ".$cv.$value;
			$change=true;
		} else if($col["col_type"]==2){
			$cv="rv".$f_id.".value_value";
			if(!empty($value)){
				$SQL_where.=" AND ".$cv."='1'";//.$value;
				$change=true;
			}
		}
	}	
	if($col["col_filter"]==2){
		if($value==-1) $SQL_where.=" AND (rv".$f_id.".value_value='' OR !rv".$f_id.".value_value)";
		else $SQL_where.=" AND rv".$f_id.".value_value='".$value."'";
		$change=true;
	}
	if($col["col_filter"]==3){
		if($col["col_link2"]==1 && is_array($value)){
			$tmp=Array();
			$mc=count($value);
			foreach($value AS $vid=>$emp)if(!empty($emp)){
				$tmp[$vid]="'".$vid."'";
			}
			$value=implode(',',$tmp);
			$SQL_where.=" AND rv".$f_id.".value_value IN (".$value.")";
			$change=true;
		}
		//checkbox single
		if($col["col_link2"]==0){
			$tmp=Array();
			foreach($value AS $vid=>$emp)if(!empty($emp)){
				$tmp[$vid]="'".$vid."'";
			}
			$value=implode(',',$tmp);
			$SQL_where.=" AND rv".$f_id.".value_value IN (".$value.")";
			$change=true;
		}
	}
}

$SQL_sort=' ro.ro_pos, ro.ro_id';
$SQL_fields='*';
global $sort;
if(!empty($sort)){
	if($sort<0){$sort_type='DESC'; $sort_id=-$sort;}
	else {$sort_type='ASC'; $sort_id=$sort;}
	$SQL_leftjoin.=' LEFT JOIN row_value AS rv ON rv.value_row=ro.row_id AND rv.value_col='.$sort_id;
	$SQL_sort='sort_field '.$sort_type.', rv.value_value '.$sort_type.', ro.ro_pos, ro.ro_id';
	$SQL_fields.=', CAST(rv.value_value AS SIGNED) AS sort_field';
}

//$SQL_owner=" AND ro.owner_id=".$id6;
//if(!empty($SQL_where)) $SQL_owner=''; //для вывода результатов подстрок в общий перечень

//тут именно так, иначе получится, что в подкатегории будут выводится разделы из совершенно другой категории
//$SQL_owner=" AND ro.ro_sub=".$id6;

//не понимаю о чём я думал, ставя здесь owner вместо sub.
//В результате все дочерние ссылки стали неактивны (дочерние клоны).
//А вот в подсчёте кол-ва строк надо именно owner, т.к. там дочерние никакого значения не играют
//Но нужно быть внимательным, т.к. теоретически, LIMIT может неправильно сработать и часть элементов остануться невидимыми (помоему такой косяк был)
//...
//Всё таки вернул owner, а ниже, видимо, надо сделать такой же рекурсиврный запрос, но с owner-ами - всеми текущими строками
//
//Пошло оно всё нафиг, буду делать как было раньше, это replacement-ы меня не устраивают. Просто надо оптимизировать коллективный seek_rlink
//Оставлю owner, но тогда они не будут фильтроваться и О ЧУДО они и не должны фильтроваться,
//для этого и был добавлен режим foption_child
//
// С прежним режимом почему-то всё виснет. Придётся действовать по плану, но тут replace_owner не требуется, т.к. мы не фильтруем дочерние страницы, значит обойдёмся простым owner-ом
if(!$foption_child) /*$SQL_owner='%replace_owner%';*//*$SQL_owner=" AND ro.ro_sub=".$id6;*/$SQL_owner=" AND ro.owner_id=".$id6; 
else $SQL_owner=" AND ro.ro_sub=".$id6;

if(!empty($id6)) $SQL_exes='';
else $SQL_exes="ro.ro_ex IN (".$exes.") AND ";
$SQL_group='';
//$SQL_group='" GROUP BY ro.row_id"';//не понятно зачем это вынесено, с этим ясен пень не пашут клонированные элементы
$SQL="SELECT ".$SQL_fields." FROM row_owner AS ro".$SQL_leftjoin." WHERE ".$SQL_exes."ro.row_table=".$table_id.$SQL_owner.$SQL_enable.$SQL_where.$SQL_group." ORDER BY ".$SQL_sort;
//молодец? поменял запрос тут? не забудь поменять его ниже!

//пересортировка строк в соответствии с выбранной сортировкой (столбцом)
if(!empty($new_sort)){
	$ns_desc='';
	$tmp_rows=getall($db,$SQL,2);
	if($new_sort<0){
		$ns_desc=' DESC';
		$new_sort=-$new_sort;
	}
	$tmp_rows_own=Array();
	if(!empty($tmp_rows)) foreach($tmp_rows AS $tmp_row){
		$tmp_rows_own[$tmp_row["owner_id"]][]=$tmp_row;
	}
	if(!empty($tmp_rows_own)) foreach($tmp_rows_own AS $tmp_array)foreach($tmp_array AS $num=>$tmp_row){
		$db->query("UPDATE row_owner SET ro_pos='".($num+1)."' WHERE ro_id=".$tmp_row["ro_id"],3,"row_owner");
	}
}

//echo $SQL;

/*$rows2*/

if(!empty($action) && $action=='filter'){
	SetCookie('limit'.$id2.$id6,$size, time()+60*60*24*30,'/');
	$_COOKIE["limit".$id2.$id6]=$size;
}
if(isset($page)){
	SetCookie('page'.$id2.$id6,$page, time()+60*60*24*30,'/');
	$_COOKIE["page".$id2.$id6]=$page;
}
if(empty($_COOKIE["limit".$id2.$id6])) $limit=30; else $limit=$_COOKIE["limit".$id2.$id6];
if(empty($_COOKIE["page".$id2.$id6])) $page=1; else $page=$_COOKIE["page".$id2.$id6];
if($page==0) $page=1;
if($limit==0) $limit=30;

if($ena==2 && $enabled<=$limit*$page){
	$page=floor($enabled/$limit);
	if($enabled % $limit==0) $page--;
	$page+=1;
}
if($ena==3 && $disabled<=$limit*$page){
	$page=floor($disabled/$limit);
	if($disabled % $limit==0) $page--;
	$page+=1;
}
$SQL_limit=' LIMIT '.$limit*($page-1).', '.$limit;
if(!$foption_child){
	if($table_multy){
		//$rows=rows_to_rlink2(getall($db,$SQL,2),0); //обычный вывод, запрос ищет только родителей, потомки ищутся с помощью seek_rlink (как попало)
		$first=true;
		$tmp=Array();
		$rows=Array();
		$links=Array();
		while($first || !empty($ids)){
			if(!$first){
				$SQL_owner=" AND ro.owner_id IN (".implode(',',$ids).")";
				$SQL="SELECT ".$SQL_fields." FROM row_owner AS ro".$SQL_leftjoin." WHERE ".$SQL_exes."ro.row_table=".$table_id.$SQL_owner.$SQL_enable.$SQL_where.$SQL_group." ORDER BY ".$SQL_sort;
				//молодец? поменял запрос тут? не забудь поменять его выше!
			}
			
			if($first && $total>$max_rows) $SQL.=$SQL_limit;
			$tmp=getall($db,$SQL,2);
			
			$ids=Array();
			if(!empty($tmp)) foreach($tmp AS $t){
				$ids[$t["row_id"]]=$t["row_id"];
				if($first) $target=&$rows[count($rows)];
				else {
					if(!isset($links[$t["owner_id"]]->sub)) $links[$t["owner_id"]]->sub=Array();
					$target=&$links[$t["owner_id"]]->sub[count($links[$t["owner_id"]]->sub)];
				}
				$target=get_rlink($t);
				$links[$t["row_id"]]=&$target;
			}
			$first=false;
		}
	} else {
		if($total>$max_rows) $SQL.=$SQL_limit;
		$rows=rows_to_rlink2(getall($db,$SQL,2),1);
	}
	//$rows=rows_to_rlink2(getall($db,$SQL,2),0); //обычный вывод, запрос ищет только родителей, потомки ищутся с помощью seek_rlink (как попало)
}
//if(!$foption_child) $rows=rows_to_rlink2(getall($db,$SQL,2),0);
else {
	if($total>$max_rows) $SQL.=$SQL_limit;
	$rows=rows_to_rlink2(getall($db,$SQL,2),1); //игнорируются потомки (т.к. они уже присутствуют тамже где родители), используется при фильтрации иерархии, когда включена опция "поиск в потомках"
}

// отсев всех элементов, кроме первого уровня
/*$rows=Array();
foreach($rows2 AS $var=>$rs){
	if($rs->owner==$id6) $rows[]=$rs;
}
unset($rows2);*/

//echo count($rows);

if($ena>1){
	$se=false;
	if($disabled>0) $se=true;
	$se2=false;
	if($enabled>0)$se2=true;
}

/*$rows=get_sub($id6,$table_id,0,1,0,0,1,$ex_ex1,$ex_ex2,$table_id,0,$table_id);

if($ena>1){
	$se=false;
	if(!empty($rows))foreach($rows AS $row) if(!$row->enable){ $se=true; break;}
	if(!$se) $ena=1;
	$se2=false;
	if(!empty($rows))foreach($rows AS $row) if($row->enable){ $se2=true; break;}
	if(!$se2) $ena=1;
	//if($ena==1) $rows=get_sub($id6,$table_id,0,1,0,0,1,$ex_ex1,$ex_ex2);
	
	if($ena==2) $rows=get_sub($id6,$table_id,1,1,0,0,1,$ex_ex1,$ex_ex2);
	if($ena==3) $rows=get_sub($id6,$table_id,2,1,0,0,1,$ex_ex1,$ex_ex2);
}
if(!empty($sort)){
	if($sort<0){$sort_type='DESC'; $sort_id=-$sort;}
	else {$sort_type='ASC'; $sort_id=$sort;}
	getrow($db,"SELECT * FROM main_col WHERE col_id=$sort_id");
	if(!empty($db->Record)){
		$rows=row_order($rows,'admin',$db->Record["col_sname"],$sort_type,2);
	}
}*/

$have_filter=false; $first=true; $fil='';
if(!empty($fcols)) foreach($fcols AS $cls){
	$have_filter=true;
	if($first){
		//table header
		$add=' style="display: none;"';
		if(!empty($f)) $add='';
		$fil.='<div id="filter"'.$add.'><div id="hider" style="padding-top: 2px; padding-bottom: 2px; height: 3px; background-color: #E6EFF6; border-bottom: 2px solid #1076DC; width: 15px; cursor: pointer;" OnClick="showhide(\'filter\');"></div>';
		global $id,$id2,$id3,$id4,$id5,$id6,$id7;
		$fil.='<form action="mod_table" method="post" style="padding: 0px; margin: 0px;">
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id3" value="'.$id3.'">
		<input type="hidden" name="id4" value="'.$id4.'">
		<input type="hidden" name="id5" value="'.$id5.'">
		<input type="hidden" name="id6" value="'.$id6.'">
		<input type="hidden" name="id7" value="'.$id7.'">
		<input type="hidden" name="sort" value="'.$sort.'">';
		$fil.='<table cellpadding=0 cellspacing=0><tr><td style="background-color: #E6EFF6; padding-left: 16px; padding-top: 16px; padding-bottom: 3px; padding-right: 16px;">';
	}
	$first=false;
	if($cls["col_filter"]==1 && $cls["col_type"]==0){
		$xv='';
		$xu=1;
		$xw=1;
		if(!empty($f[$cls["col_id"]])) $xv=$f[$cls["col_id"]];
		if(!empty($uc[$cls["col_id"]])) $xu=$uc[$cls["col_id"]];
		if(!empty($wh[$cls["col_id"]])) $xw=$wh[$cls["col_id"]];
		$xn1='.a.';
		$xn1t='частичное совпадение';
		if($xw==2){
			$xn1="'a'";
			$xn1t='полное совпадение';
		}
		if($xw==3){
			$xn1="<s>.a.</s>";
			$xn1t='не содержит (частично)';
		}
		$xn2='aA';
		$xn2t='с учётом регистра';
		if($xu==2){
			$xn2="aa";
			$xn2t='без учёта регистра';
		}
		$fil.='<div style="float: left; margin-right: 16px;">'.$cls["col_name"].'<br>
			<input type="text" name="f['.$cls["col_id"].']" style="width: 200px; float: left;" value="'.$xv.'">
			<input type="hidden" name="uc['.$cls["col_id"].']" id="uc'.$cls["col_id"].'" value="'.$xu.'">
			<input type="hidden" name="wh['.$cls["col_id"].']" id="wh'.$cls["col_id"].'" value="'.$xw.'">
			<div style="float: right; width: 45px; height: 16px; margin-top: 5px;">
				<div title="'.$xn1t.'" class="switch_btn" style="margin-left: 3px;" align="center" OnClick="fclick2(this,'.$cls["col_id"].');" OnMouseUp="remove_selection();">'.$xn1.'</div>
				<div title="'.$xn2t.'" class="switch_btn" align="center" OnClick="fclick1(this,'.$cls["col_id"].');" OnMouseUp="remove_selection();">'.$xn2.'</div>
			</div>
		</div>';
	}
	if($cls["col_filter"]==1 && $cls["col_type"]==2){
		$xv='';
		$xu=1;
		$xw=1;
		$xv2='';
		if(!empty($f[$cls["col_id"]])) $xv=$f[$cls["col_id"]];
		if($xv!='') $xv2=' checked';
		$fil.='<div style="float: left; margin-right: 16px;">'.$cls["col_name"].'<br>
			<input type="checkbox" class="checkbox" name="f['.$cls["col_id"].']"'.$xv2.'>
			<input type="hidden" name="uc['.$cls["col_id"].']" id="uc'.$cls["col_id"].'" value="'.$xu.'">
			<input type="hidden" name="wh['.$cls["col_id"].']" id="wh'.$cls["col_id"].'" value="'.$xw.'">
		</div>';
	}
	if($cls["col_filter"]==3 || $cls["col_filter"]==2){
		//копировано из func2
		foreach($cls AS $var=>$value) $$var=$value;
		$modex=$ex_ex2v;
		$skip_ex=0;
		if($col_link3!=2) $seek_subt=1; else $seek_subt=0;
		$tmp=Array();
		if($col_link>0){
			$tex=get_tex(0,$modex,$col_link);
			if($skip_ex==0) $tmp=get_sub(0,$col_link,1,1,1,0,$seek_subt,$tex,$modex,$table_id,0,$col_deep,1);
			else $tmp=get_sub(0,$col_link,1,0,1,0,$seek_subt,$tex,$modex,$table_id,0,$col_deep,1);
		} else if($col_link!=0){
			$cl=-$col_link;
			global $id6;
			$tti=$id6;
			$tex=get_tex(0,$modex,$cl);
			$tmp=get_sub($tti,$cl,1,1,0,0,$seek_subt,$tex,$modex,$table_id,1);
			if(empty($tmp) && !empty($arow)){
						$tti=$arow;
						$tmp=get_sub($tti,$cl,1,1,0,0,$seek_subt,$tex,$modex,$table_id,1,$col_deep,1);
			}
		} else if(!empty($col_speclink)){
			$tmp=parse_var($col_speclink);
			if(is_object($tmp) && isset($tmp->rows)) $tmp=$tmp->rows;
			if(!is_array($tmp)) $tmp=Array();
		}
		$rs=get_vars2($tmp);		
	}
	if($cls["col_filter"]==2 && !empty($rs)){
		$fil.='<div style="float: left; margin-right: 16px;">'.$cls["col_name"].'<br>';
		$fil.='<select name="f['.$col_id.']" style="width: 200px;"><option value="0">Выберите значение</option>';
		if(empty($col_default)) $col_default=0; if(!empty($f[$col_id])) $col_default=$f[$col_id];
		$fil.=options($rs,' ',$col_default,1,0);
		$fil.='<option value="-1"'.($col_default=='-1'?' selected':'').'>Пустое</option>';
		$fil.='</select>';
		$fil.='</div>';
	}
	if($cls["col_filter"]==3 && !empty($rs)){
		$fil.='<div style="float: left; margin-right: 16px;">'.$cls["col_name"];
		if($cls["col_link2"]==0) $fil.=' (или)'; else $fil.=' (и)';		
		$level=seek_max_level($rs);
		if($col_link3!=3) $level=0;
		if(!isset($f[$col_id]) || !is_array($f[$col_id])) $col_default=Array(); else {
			$col_default=Array();
			foreach($f[$col_id] AS $vid=>$emp) if(!empty($emp)) $col_default[$vid]=$vid;
		}
		$fil.='<div style="margin-top: 4px; padding: 5px; background-color: #F9F9FF; border: 1px solid #A5A5A5;">';
		$fil.=checkbox($rs,'','f['.$col_id.']',$col_link3,array_flip($col_default),0,$level);			
		$fil.='</div>';
		$fil.='</div>';
	}
}
if($have_filter){
	//table bottom
	
	$fil.='
	<script>
	function select_filter_child(o){
		if(o.src.indexOf("'.$base_root.'/files/editor/tree.png")!=-1){
			o.src="'.$base_root.'/files/editor/tree2.png";
			o.style.backgroundColor="#EAEAF9";
			document.getElementById("treef2").value=1;
		} else {
			o.src="'.$base_root.'/files/editor/tree.png";
			o.style.backgroundColor="#F9F9FF";
			document.getElementById("treef2").value=0;
		}
	}
	</script>	
	';
		
	$fil.='</td></tr><tr><td style="background-image: url(/files/editor/filter_bg.png); background-repeat: repeat-x;" valign="middle" height="24">';
	$fil.='<div align="right" style="padding-right: 19px; margin-bottom: 10px;">';
	
	if($table_multy){
		if(!$foption_child){
			$fil.=se('tree','','',' class="link" style="margin-right: 10px; padding: 2px; border: 1px solid #A5A5A5; background-color: #F9F9FF; cursor: pointer;" align="absmiddle" id="treef" title="Искать в потомках" OnClick="select_filter_child(this);"',0,2);
			$fil.='<input type="hidden" id="treef2" name="f[option_child]" value="0">';
		} else {
			$fil.=se('tree2','','',' class="link" style="margin-right: 10px; padding: 2px; border: 1px solid #A5A5A5; background-color: #EAEAF9; cursor: pointer;" align="absmiddle" id="treef" title="Искать в потомках" OnClick="select_filter_child(this);"',0,2);
			$fil.='<input type="hidden" id="treef2" name="f[option_child]" value="1">';
		}
	}
	
	
	$fil.='<input type="submit" name="sbt" value="Искать" style="width: 120px; cursor: pointer; margin-top: 2px; margin-right: 10px;">';
	if(!empty($f)) $fil.='<input type="submit" name="sbt" value="Отмена" style="width: 120px; margin-right: 10px; cursor: pointer; margin-top: 2px;">';
	$fil.='</div>';
	$fil.='</td></tr></table></form></div>';
}

/*if(!empty($f) && !empty($rows)){
	$r=collect_rows($rows);
	$r=implode(',',$r);
	$br=Array();
	if(is_array($f) && !empty($f)) foreach($f AS $col_id=>$value)if(!empty($value) && isset($fcols[$col_id]) && $r!=''){
		$sql="SELECT value_row, value_col, value_value FROM row_value WHERE value_row IN ($r) AND value_col=".$col_id;
		$change=false;
		$col=$fcols[$col_id];
		if($col["col_filter"]==1){
			if($col["col_type"]==0){
				//текст
				$cv="value_value";
				if($uc[$col_id]==2){
					$value=strtolower($value);
					$cv="LOWER(value_value)";
				}
				if($wh[$col_id]==1) $value=" LIKE '%".$value."%'"; else if($wh[$col_id]==2) $value="='".$value."'";
				else $value=" NOT LIKE '%".$value."%'";
				$sql.=" AND ".$cv.$value;
				$change=true;
			} else if($col["col_type"]==2){
				if(!empty($value)){
					$sql.=" AND value_value='1'";
					$change=true;
				}
			}
		}	
		if($col["col_filter"]==2){
			$sql.=" AND value_value='".$value."'";
			$change=true;
		}
		if($col["col_filter"]!=3){
			$r=getall3($db,$sql,"value_row");
			$br=$r;
			$r=implode(',',$r);
			$change=true;
		} else {
			//checkbox multy
			if($col["col_link2"]==1 && is_array($value)){
				$tmp=Array();
				$mc=count($value);
				foreach($value AS $vid=>$emp)if(!empty($emp)){
					$tmp[$vid]="'".$vid."'";
				}
				$value=implode(',',$tmp);
				$sql.=" AND value_value IN (".$value.")";
				$tmp=getall($db,$sql,1,"row_value");
				$rs=Array();
				$r=Array();
				foreach($tmp AS $tm){
					$rs[$tm["value_row"]][$tm["value_value"]]=1;
					if(count($rs[$tm["value_row"]])==$mc) $r[$tm["value_row"]]=$tm["value_row"];
				}
				$br=$r;
				$r=implode(',',$r);
				$change=true;
			}
			//checkbox single
			if($col["col_link2"]==0){
				$tmp=Array();
				foreach($value AS $vid=>$emp)if(!empty($emp)){
					$tmp[$vid]="'".$vid."'";
				}
				$value=implode(',',$tmp);
				$sql.=" AND value_value IN (".$value.")";
				$r=getall3($db,$sql,"value_row");
				$br=$r;
				$r=implode(',',$r);
				$change=true;
			}
		}
	}
	if($change) $rows=filter_rows($rows,$br,1);
}*/

$rows2=$rows;
$rows=Array();
foreach($rows2 AS $row){
	if(check_row($row->id,$table_id,$ex_ex2v,'view',$row->user,$row->users)) $rows[]=$row;
	else if($total>$max_rows) $rows[]='hidden';
}
if(!isset($se)){
	if(!empty($rows))foreach($rows AS $row) if(is_object($row) && !$row->enable){ $se=true; break;}
}
if(!isset($se2)){
	if(!empty($rows))foreach($rows AS $row) if(is_object($row) && !$row->enable){ $se2=true; break;}
}
if($total>$max_rows){
	if($ena==1) $crows=$total;
	else if($ena==2) $crows=$enabled;
	else if($ena==3) $crows=$disabled;
} else $crows=count($rows);

if($crows>30){
	$cp=floor($crows/$limit);
	if($crows % $limit!=0) $cp++;
	if($page>$cp) $page=$cp;
	if(!empty($_POST["action"]) && $_POST["action"]=='add_row'){
		$page=$cp;
		SetCookie('page'.$id2.$id6,$page, time()+60*60*24*30,'/');
		$_COOKIE["page".$id2.$id6]=$page;
	}
	if($total<=$max_rows){
		$rows2=$rows;
		$GLOBALS["a_rows"]=$rows;
		$rows=Array();
		for($i=0;$i<$limit;$i++){
			if(isset($rows2[($page-1)*$limit+$i])) $rows[]=$rows2[($page-1)*$limit+$i];
		}
	} else {
		$GLOBALS["a_rows"]=$rows;
	}
} else $GLOBALS["a_rows"]=$rows;


$GLOBALS["a_rows2"]=$rows;

//echo 'se - '.$se.'; se2 - '.$se2.'; ena - '.$ena.'<br>';

if(count($rows)==0){
	$fil=del_tag($fil,'<div id="hider"','</div>');
}

echo $fil;

echo '<table width="100%" cellpadding="0" cellspacing="0"><tr><td><div style="float: right; height: 21px; margin-bottom: 7px;">';
if(empty($GLOBALS["buff"]) && !empty($_COOKIE["buff"])) $GLOBALS["buff"]=$_COOKIE["buff"];
echo '<span id="buff" style="display: none;"><span class="link" OnClick="document.tableform.chk_type.selectedIndex=5; document.tableform.submit();">'.si('buffer_new').'</span>';
if(!empty($GLOBALS["buff"]) && strstr($GLOBALS["buff"],$table_id.'**') && $can_add){
	echo '<span class="link" OnClick="document.tableform.chk_type.selectedIndex=6; document.tableform.submit();">'.si('buffer_add').'</span>';
	echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>';
	$href="mod_table?id=$id&amp;id2=$id2&amp;id6=$id6&amp;id7=$id7".$df."&amp;action=";
	echo se('buffer_move',$href.'buffer_move');
	echo se('buffer_copy',$href.'buffer_copy');
	getrow($db,"SELECT * FROM table_sub WHERE sub_table1=$table_id",1,"table_sub");
	if(!empty($db->Record))	{
		echo se('buffer_copy2',$href.'buffer_copy2');
		echo se('buffer_copy3',$href.'buffer_copy3');
	}
	echo se('buffer_clone',$href.'buffer_clone');
	echo se('buffer_clear',$href.'buffer_clear');
}	else echo '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>';
echo '</div>';

// Навигация: Показать фильтр

echo '<div style="float: left; margin-bottom: 7px;"><table cellpadding="0" cellspacing="0"><tr><td valign="middle">';
if($have_filter){	
	echo '<div class="link" style="float: left; margin-right: 12px;" class="link" OnClick="showhide(\'filter\');">'.si('filter',0,0,'').'</div>';
}

// Навигация: Показывать Активные / Неактивные / Все строки

if(!empty($se) && $se && !empty($se2) && $se2 || ($total>$max_rows && $disabled && $enabled)){
	$href="mod_table?id=$id&amp;id2=$id2&amp;id6=$id6&amp;id7=$id7".$df."&amp;sort=$sort&amp;action=";
	if($ena!=1) echo se('all2',$href.'show_all'); else echo si('all1');
	if($ena!=2) echo se('active2',$href.'show_enable'); else echo si('active1');
	if($ena!=3) echo se('deactive2',$href.'show_disable'); else echo si('deactive1');
}

if($crows>0){
	echo '</td><td valign="top" align="left">';
	$start=0;$end=$crows-1;
	if($crows>30){
	
		// Навигация: Пагинатор
		
		$href="mod_table?id=$id&amp;id2=$id2&amp;id6=$id6&amp;id7=$id7".$df;
		if(isset($sort)) $href.='&amp;sort='.$sort;
		$href.='&amp;page=';
		$max=$crows;
		//$start=0;$end=count($rows)-1;
		echo '<div style="margin-right: 15px; margin-left: 5px;">';
		if($max>$limit){
			echo '<div class="pagebar">';
			$cp=floor($max/$limit);
			if($max%$limit!=0) $cp++;
			if($page>$cp) $page=$cp;
			if(!empty($_POST["action"]) && $_POST["action"]=='add_row'){
				$page=$cp;
				SetCookie('page'.$id2.$id6,$page, time()+60*60*24*30,'/');
				$_COOKIE["page".$id2.$id6]=$page;
			}
			$num=24;
			$p_start=1;
			$p_end=$cp+1;
			if($page>$num/2+1){
				$p_start=$page-$num/2+1;
				//echo '<div class="page-box link" style="margin-right: 30px;" OnClick="document.location.href=\''.$href.(1).'\';" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.(1).'" class="trueblue">1</a></div>';
				echo '<div class="page-box link" style="margin-right: 30px;" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.(1).'" class="trueblue">1</a></div>';
			}
			if($page<$cp-$num/2-1){
				$p_end=$page+$num/2+1;
			}
			if($p_start>$cp) $p_start=$cp;
			//if($p_end>$cp) $p_end=$cp;
			if($p_end<1) $p_end=1;
			if($p_start<1) $p_start=1;
			if($p_start>$p_end) $p_start=$p_end;
			for($i=$p_start-1;$i<$p_end-1;$i++){
				//if($i%30==0 && $i>0) echo '</div><div class="pagebar" style="margin-top: 5px;">';
				//if($i+1==$page) echo '<div class="page-box pb-active">'; else echo '<div class="page-box link" OnClick="document.location.href=\''.$href.($i+1).'\';" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.($i+1).'" class="trueblue">';
				if($i+1==$page) echo '<div class="page-box pb-active">'; else echo '<div class="page-box link" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.($i+1).'" class="trueblue">';
				echo ($i+1);
				/*if($i<$cp-1) echo ', ';*/
				if($i+1==$page) echo '</div>'; else echo '</a></div>';
			}
			if($page<$cp-$num/2-1){
				echo ' <div class="page-box link" style="margin-left: 30px;" OnClick="document.location.href=\''.$href.($cp).'\';" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.($cp).'" class="trueblue">'.$cp.'</a></div>';
			}
			$start=($page-1)*$limit;
			$end=$start+$limit-1;
			echo '</div>';
		}
		echo '</div>';
		echo '</td><td>';
		echo '<form name="tableform2" action="mod_table" method="post" style="margin: 0px; padding: 0px;">'.$df2.'
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id6" value="'.$id6.'">
		<input type="hidden" name="id7" value="'.$id7.'">
		<input type="hidden" name="sort" value="'.$sort.'">
		<input type="hidden" name="action" value="filter">';
		echo '<input type="text" name="size" value="'.$limit.'" class="mini_input"><input type="submit" class="arrow_btn" value=">">';
		echo '</form>';
	}
	echo '</td></tr></table></div></td></tr></table>';
	
	//$se=false;
	//if($ena!=1) $se=true; else
	
	// Загрузка родителей / смежных разделов / текущих строк (для поля Родитель)
	
	$cols=getall($db,"SELECT * FROM main_col WHERE col_table=$table_id AND col_bold=1 ORDER BY col_pos",1,'main_col');
	global $theader,$rub_opt,$global_rows;
	if($table_multy) $rub_opt=get_vars2(get_sub($id6,$table_id,0,1,0,0,1,$ex_ex1,$ex_ex2,$table_id,0,$table_id,1));
	if(!empty($id6)){
		$global_rows=get_vars2(get_sub($mrow,$up_table_id,0,1,0,0,1,$ex_ex1,$ex_ex2,$up_table_id,0,$up_table_id,1));
		//echo count($global_rows[0]->sub);
	}
	
	// Загрузка блока пользователей
	
	//$sel_user=select_users(0,($user->super?1:0),1);
	$sel_user=' some data ';
	//echo '<div id="usel" style="display: none;"><select id="se%id%" name="new_user" style="margin: 0px; margin-top: 5px;">'.$sel_user.'</select><div style="padding-top: 5px; padding-bottom: 5px;"><a href="row_user?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.'&id3=%id%">доп. владельцы%sv%</a></div><input type="button" class="button" value="сменить" style="margin: 0px;"
	echo '<div id="usel" style="display: none;"><div id="usel-container"></div><div style="padding-top: 5px; padding-bottom: 5px;"><a href="row_user?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.'&id3=%id%">доп. владельцы%sv%</a></div><input type="button" class="button" value="сменить" style="margin: 0px;"
	OnClick="var obj=document.getElementById(\'se%id%\'); document.location.href=\'mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.$df3.'&action=new_user&id3=%id%&new_user=\'+obj.value;"></form></div>';
	
	// Загрузка блока подчинений
	
	if(!$table_multy)/*не уверен в этом условии*/ $opts=options($rub_opt,'',0,1,2);
	if($table_multy){
		echo '<script>
		var list1=\'<br>подчинить строке:<br><select id="tid\';
		var list2=\'" class="button"><option value="0">Без подчинения</option>\';
		var list3a=[';
		$opts=options($rub_opt,'',0,1,2);$i=0;
		if(!empty($opts)) foreach($opts AS $opt){
			if($i!=0) echo ','; echo $opt->index;
			//echo ' list3['.$i.'][0]='.$opt->index.'; ';
			//echo ' list3['.$i.'][1]=\''.$opt->data.'\'; ';
			$i++;
		}
		echo '];
		var list3b=[';
		$i=0;
		if(!empty($opts)) foreach($opts AS $opt){
			if($i!=0) echo ','; echo '\''.safe_sql_input($opt->data,2).'\'';
			$i++;
		}
		echo ']; ';
		echo 'var list3c=[];';
		$i=0;
		if(!empty($opts)) foreach($opts AS $opt){
			echo ' list3c['.$i.']=[';
			$i2=0;
			if(!empty($opt->own)) foreach($opt->own AS $var=>$value){
				if($i2!=0) echo ',';
				echo $var;
				$i2++;
			}
			echo ']; ';
			$i++;
		}
		echo ' var list4=\'</select>\';';
		echo ' var list5=\'\'; var list6=\'\';';
		$itir='';
		if(!empty($id6)){
			echo 'list5=\'<br>или глобально</b>:<br><select id="tir\';';
			//echo 'list5.=$tid
			echo 'list6=\'" class="button"><option value="0">Нет</option>';
			options($global_rows,' ',0,1,1);
			echo '</select>\';';
			//$itir='+\'&id5=\'+document.getElementById(\'tir'.$tid.'\').value';
		}
		echo ' var list7=\'<br><input type="button" class="button" value="клонировать" OnClick="document.location.href=\\\'mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.$df3.'&action=add_parent&id3=\';';
		//$row->id.
		echo ' var list8=\'&id4=\\\'+document.getElementById(\\\'tid\';';
		//$tid		
		echo ' var list9=\'\\\').value\';';
		echo ' var list10=\';">\';';
		echo '</script>';
	}
	
	// Подгрузка плагина перетаскивания строк
	
	echo '<script>$(document).ready(function() { $(".rcrds").tableDnD({      onDragClass: "drag1",      dragHandle: "drag2" });  });</script>';
	
	// Начало формы для быстрых действий с объектами
	
	echo '<form name="tableform" action="mod_table" method="post" enctype="multipart/form-data">'.$df2.'
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="id2" value="'.$id2.'">
	<input type="hidden" name="id6" value="'.$id6.'">
	<input type="hidden" name="id7" value="'.$id7.'">';
	if(isset($sort)) echo '<input type="hidden" name="sort" value="'.$sort.'">';
	if(isset($df2)) echo $df2;//'<input type="hidden" name="sort" value="'.$df2.'">';
	echo '<input type="hidden" name="action" value="resort">
	<table id="records" cellpadding="3" cellspacing="1" class="rcrds">';
	$theader='<tr style="" class="noDrop">';
	//$theader.='<th width="45">№<![if !IE]> <input type="checkbox" class="checkbox1b" OnClick="chkall(this);show(\'chpanel\');show(\'chpanel2\');"><![endif]></th>';
	$theader.='<th width="45" colspan="2">№ <input type="checkbox" class="checkbox1b" OnClick="chkall(this);show(\'chpanel\');show(\'buff\');show(\'chpanel2\');"></th>';	
	
	// Загрузка списка подтаблиц
	
	$tsubs2=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$table_id",1,'table_sub');
	$in2='';$tsubs=Array();if(!empty($tsubs2)) foreach($tsubs2 AS $tsub2){if(!empty($in2)) $in2.=','; $in2.=$tsub2["sub_table2"];}
	$tsubs_add=Array();
	if(!empty($in2)){
		$tsubs2=getall($db,"SELECT * FROM main_table WHERE table_id IN (".$in2.") ORDER BY table_name",1,'main_table');
		//if(!empty($tsubs2)) foreach($tsubs2 AS $tsub2)$tsubs[$tsub2["table_id"]]=$tsub2["table_name"];
		// сверху вариант - он подразумевает запрет на редактирование подтаблиц строки, которая защищёна
		// снизу вариант - подразумевает запрет на редактирование запрещённых модулей
		if(!empty($tsubs2)) foreach($tsubs2 AS $tsub2){
			//if(empty($id6)){
			//	if(check_tbl($tsub2["table_id"],'view') || check_tbl($tsub2["table_id"],'add')){
			//		$tsubs[$tsub2["table_id"]]=$tsub2["table_name"];
			//	}
			//} else {
				if(check_tbl(get_st($id6,$table_id,$tsub2["table_id"]),'view')){//не совсем понятное условие...
					$tsubs[$tsub2["table_id"]]=$tsub2["table_name"];
					$tsubs_add[$tsub2["table_id"]]=$tsub2["table_id"];
				}
			//}
		}
	}
	
	unset($crows);
	
	// Загрузка шапки таблицы (заголовки столбцов)
	
	foreach($cols AS $col)if(check_col($col["col_id"],'view')){
		if($col["col_fastedit"] && $col["col_type"]==1 && $col["col_link2"]==0){
			$tmp=Array();
			$col_link3=$col["col_link3"];
			$col_link=$col["col_link"];
			$col_deep=$col["col_deep"];
			$ctable=$col["col_table"];
			if($col_link3!=2) $seek_subt=1; else $seek_subt=0;
			$skip_ex=0;
			$ctable_id=$ctable;
			if($col_link>0){
				$tex=get_tex(0,$ex_ex2v,$col_link);
				if($skip_ex==0) $tmp=get_sub(0,$col_link,1,1,1,0,$seek_subt,$tex,$ex_ex2v,$ctable_id,0,$col_deep,1);
				else $tmp=get_sub(0,$col_link,1,0,1,0,$seek_subt,$tex,$ex_ex2v,$ctable_id,0,$col_deep,1);
			} else {
				$cl=-$col_link;
				global $id6;
				$tti=$id6;
				$tex=get_tex(0,$ex_ex2v,$cl);
				$tmp=get_sub($tti,$cl,1,1,1,0,$seek_subt,$tex,$modex,$ctable_id,1);
				if(empty($tmp) && !empty($arow)){
							$tti=$arow;
							$tmp=get_sub($tti,$cl,1,1,1,0,$seek_subt,$tex,$ex_ex2v,$ctable_id,1,$col_deep,1);
				}
			}
		
			$crows[$col["col_id"]]=get_vars2($tmp);//get_vars2(get_sub(0,$col["col_link"],1,1,1,0,1,$ex_ex1,$ex_ex2,0,0,1));
			$crows_c[$col["col_id"]]=collect_rows3($crows[$col["col_id"]]);
		}
		if($col["col_type"]==0 || ($col["col_type"]==1 && $col["col_link2"]==0) || $col["col_type"]==2 || $col["col_type"]==5){
			//тут необходимо учитывать настройки фильтра и может добавить ?page=0, т.к. страница сидит в куках
			$def_url='?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7;
			$sort_url=$def_url.'&amp;sort='.$col["col_id"].'&amp;page=0';
			if(!empty($df)) $sort_url.=/*'&amp;f='.*/$df;
			if(!empty($sort) && ($sort==$col["col_id"] || $sort==-$col["col_id"])){
				if($sort==$col["col_id"]){
					$sort_img='sort.png';
					$sort_url=$def_url.'&amp;sort=-'.$col["col_id"].'&amp;page=0';
					if(!empty($df)) $sort_url.=/*'&amp;f='.*/$df;
				} else {
					$sort_img='backsort.png';
					$sort_url=$def_url.'&amp;page=0';//возврат к сортировке по номеру
					if(!empty($df)) $sort_url.=/*'&amp;f='.*/$df;
				}
				$theader.='<th><a href="'.$sort_url.'" style="color: #FFFFFF;"><img src="'.$GLOBALS["base_root"].'/files/editor/'.$sort_img.'" align="absmiddle" style="margin-right: 5px;" border=0>'.$col["col_name"].'</th>';
			} else $theader.='<th><a href="'.$sort_url.'" style="color: #FFFFFF;">'.$col["col_name"].'</th>';
		} else $theader.='<th>'.$col["col_name"].'</th>';
	}
	
	//	подгрузка частей для выполнения над строкой (тип 5)
	
	$sprts=getall($db,"SELECT * FROM main_part WHERE part_type=5 AND part_table=$table_id",1,"main_part");
	$spec_act=Array();$sid=0;
	if(!empty($sprts)) foreach($sprts AS $sprt){
		$spec_act[$sid]->id=$sprt["part_id"];
		$spec_act[$sid]->pic=$sprt["part_pic"];
		$spec_act[$sid]->name=$sprt["part_name"];
		$spec_act[$sid]->if=$sprt["part_ifcase"];
		$spec_act[$sid]->self=$sprt;
		$sid++;
	}
	
	//	шапка таблицы
	if(!empty($tsubs)) $theader.='<th>Подразделы</th>';
	$theader.='<th>Действия</th>';
	$theader.='</tr>';
	echo $theader;
	echo '<tbody>';
	
	//	подготовка для tree optimize
	$GLOBALS["spec_step2"]=0;
	$GLOBALS["spec_step"]=0;	
	global $firstt;
	$firstt=true;
	if($firstt){
		$GLOBALS["tree_vars"][$GLOBALS["spec_step2"]]=Array();
	}	
	
	//	загрузка строк
	$rows=load_rows($rows,$cols,$ena);
	//sort_rows($rows);
	
	//подгрузка таблицы URL для $GLOBALS["start_url"]
	if(!empty($GLOBALS["start_url2"])){
		$uc=url_col($table_id);
		if(!empty($uc)){
			$rws=collect_rows($rows);
			if(!empty($rws)){
				$GLOBALS["url_rows"]=getall6($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$rws).") AND value_col=".$uc["col_id"],"value_row","value_value");
			}
		}
	}
	
	// вывод строк
	$start=0;
	if(!empty($limit)) $end=$limit-1;
	show_rows($rows,$cols,0,0,$start,$end,$ena);
	
	// подвал таблицы
	echo '</tbody>';
	echo '</table>';
	echo "<script>
		function onsel(obj){
			var obj2=document.getElementById('selex');
			if(obj2){
				if(obj.value==4) obj2.style.display='';
				else obj2.style.display='none';
			}
			var obj2=document.getElementById('selown');
			if(obj2){
				if(obj.value==5) obj2.style.display='';
				else obj2.style.display='none';
			}
			var obj2=document.getElementById('selpart');
			if(obj2){
				if(obj.value==6) obj2.style.display='';
				else obj2.style.display='none';
			}
			var obj2=document.getElementById('selown2');
			if(obj2){
				var obj_chuser=obj2;
				//смена владельца
				if($('#usel-container').html()==''){
					load_users(function(){
						if(obj.value==9) obj_chuser.style.display='';
						else obj_chuser.style.display='none';
					});
				} else {
					if(obj.value==9) obj2.style.display='';
					else obj2.style.display='none';
				}
			}
			var obj2=document.getElementById('selbuff');
			if(obj2){
				if(obj.value==11) obj2.style.display='';
				else obj2.style.display='none';
			}
			var obj2=document.getElementById('selcol');
			if(obj2){
				if(obj.value==12) obj2.style.display='';
				else obj2.style.display='none';
			}
		}
		</script>";
	if(!empty($sort) && empty($new_sort)){
		global $page;
		echo '<div style="float: left; margin-right: 20px;"><input type="button" value="Сохранить порядок строк согласно сортировки" OnClick="document.location.href=\'mod_table?id='.$id.'&id2='.$id2.'&id6='.$id6.'&id7='.$id7.'&sort='.$sort.'&new_sort='.$sort.$df.'&page='.$page.'\'"></div>';
	}				
	echo '<div id="chpanel" style="display: none;"><input type="submit" value="Применить изменения" class="button"> <input type="reset" value="Сбросить" class="button" OnClick="hide(\'chpanel\');hide(\'chpanel2\');">
		<span id="chpanel2" style="display: none;">с отмеченными:
		<select name="chk_type" class="button" OnChange="JavaScript: onsel(this);">
			<option value="0">Ничего не делать</option>
			<option value="1">Удалить</option>
			<option value="2">Активировать</option>
			<option value="3">Деактивировать</option>
			<option value="10">Экспортировать</option>
			<option value="7">В буфер обмена</option>';
			if(!empty($GLOBALS["buff"]) && strstr($GLOBALS["buff"],$table_id.'**')) echo '<option value="8">Добавить к текущему буферу</option>';
	echo '	<option value="12">Установить значение</option>';
	
	// Установить значение
	$sel8='<span id="selcol" style="display: none;"> -> <select name="selcol" class="button" OnChange="
		var obj=document.getElementById(\'c\'+old_c);
		obj.style.display=\'none\';
		old_c=this.value;
		var obj=document.getElementById(\'c\'+old_c);
		obj.style.display=\'\';
	">';
	$sel8b='';
	$cols=getall($db,"SELECT col_name, col_id, col_inform, col_table, col_pos, col_type FROM main_col WHERE col_table=$table_id AND col_inform=1 ORDER BY col_pos");
	$first=true;
	if(!empty($cols)) foreach($cols AS $col)if($col["col_type"]!=3){
		$sel8.='<option value="'.$col["col_id"].'">'.$col["col_name"].'</option>';
		//тут надо оптимизировать, чтобы небыло повторного запроса к COL
		if($first) $sel8b.='<script>var old_c='.$col["col_id"].';</script>';
		$sel8b.='<div id="c'.$col["col_id"].'" style="margin-top: 10px; width: 80%; border: 1px solid #999999; padding: 10px; '.(!$first?'display:none;':'').'">'.input_form($table_id,$module_id,0,$col["col_id"],$id2,$ex_ex2,0,0,Array(),'fastedit_',0).'</div>';
		$first=false;
	}
	$sel8.='</select>'.$sel8b.'</span>';
	
	$sel7='';
	if(!empty($tsubs) && !empty($GLOBALS["buff"])){
		$first=true;
		foreach($tsubs AS $t_id=>$t_name) if(strstr($GLOBALS["buff"],$t_id.'**')){
			if($first){
				echo '<option value="11">Вставить из буфера в подтаблицу</option>';
				$sel7='<span id="selbuff" style="display:none;"> -> <select name="selbuff" class="button">';
			}
			$first=false;
			$sel7.='<option value="'.$t_id.'">'.$t_name.'</option>';
		}
		if(!$first) $sel7.='</select></span>';
	}
			
	$sel6='';
	if(!empty($sel_user)){
		echo '<option value="9">Сменить владельца</option>';
		$sel6='<span id="selown2" style="display:none;"> -> <select name="new_own2" id="new_own2" class="button">';
		$sel6.=$sel_user;
		$sel6.='</select></span>';
	}
	$sel3='';
	if($table_multy && !empty($opts)){
		echo '<option value="5">Сменить родителя</option>';
		$sel3='<span id="selown" style="display:none;"> -> <select name="new_own" class="button">';
		$sel3.= '<option value="0">Без родителя</option>';
		foreach($opts AS $opt){
			$sel3.=$opt->data;//'<option value="'.$tex["ex_ex1"].'">'.$exs[$tex["ex_ex2"]]["ex_name"].'</option>';
		}
		$sel3.='</select></span>';
	}			
	$sel2='';
	if(empty($id6)){
		$sel2='';
		$texs2=getall($db,"SELECT * FROM ex_group WHERE ex_module=$id AND ex_table=$table_id AND ex_ex1!=$id2",1,"ex_group");
		$texs=Array();
		foreach($texs2 AS $tex)if(check_ex($tex["ex_ex2"],'view')) $texs[]=$tex;
		if(!empty($texs)){
			echo '<option value="4">Сменить раздел</option>';
			$sel2='<span id="selex" style="display:none;"> -> <select name="new_ex" class="button">';
			foreach($texs AS $tex){
				$sel2.='<option value="'.$tex["ex_ex1"].'">'.$exs[$tex["ex_ex2"]]["ex_name"].'</option>';
			}
			$sel2.='</select></span>';
		}
	}
	if(!empty($id6)){
		$sel2='';
		$opts=options($global_rows,' ',$id6,1,2/*,$id6*/);
		if(!empty($opts)){
			echo '<option value="4">Сменить раздел</option>';
			$sel2='<span id="selex" style="display:none;"> -> <select name="new_ownB" class="button">';
			foreach($opts AS $opt){
				$sel2.=$opt->data;
			}
			$sel2.='</select></span>';
		}
	}
	$sel4='';
	if(!empty($spec_act)){
		echo '<option value="6">Выполнить действие</option>';
		$sel4='<span id="selpart" style="display:none;"> -> <select name="part" class="button">';
		foreach($spec_act AS $sa){
			$sel4.='<option value="'.$sa->id.'">'.$sa->name.'</option>';
		}
		$sel4.='</select></span>';
	}
	echo '	</select>'.$sel3.$sel4.$sel6.$sel7.$sel2.$sel8.'</span></div></form>';
	global $chk;
	echo '<script>function chkall(chb){';
	echo 'var obj=document.getElementsByClassName("checkbox1");';
	echo 'if(obj.length==0) obj=document.getElementsByClassName("checkbox2"); if(obj.length==0) return "";';
	echo 'for (var key in obj) {
			 var val = obj[key];
			 val.checked=chb.checked;
			 selr2(val.id,chb.checked);
		 }';
	//if(!empty($chk)) foreach($chk AS $var2=>$value) echo ' document.tableform["'.$var2.'"].checked=chb.checked; selr2(\''.$value.'\',chb.checked);';
	echo '}</script>';
} else echo '</td></tr></table></div></td></tr></table>';

echo show_se();

//==========================
//  Форма добавления строки
//==========================
global $action;


/*if($action=='edit_form') seek_rlink($id3);//getrow($db,"SELECT * FROM main_row WHERE row_id=$id3");
if($action!='edit_form' && !empty($id6)) seek_rlink($id6);//getrow($db,"SELECT * FROM main_row WHERE row_id=$id6");

if(($action!='edit_form' && empty($id6) && check_row(0,$ex_table2,$ex_ex2v,'add'))
||($action!='edit_form' && !empty($id6) && check_row($id6,$rlink[$id6]->table,$ex_ex2v,'edit',$rlink[$id6]->user,$rlink[$id6]->users))
||($action=='edit_form' && check_row($id3,$ex_table2,$ex_ex2v,'edit',$rlink[$id3]->user,$rlink[$id3]->users))
){*/
if(!isset($id3)) $id3=0;
if(($action!='edit_form' && check_operation('add',$id3,$id6,$ex_ex2v,$ex_table2)) || ($action=='edit_form' && check_operation('edit',$id3,$id6,$ex_ex2v,$ex_table2))){

echo '<a name="form"></a>';
if($action!='edit_form') $GLOBALS["f_action"]='add'; else  $GLOBALS["f_action"]='edit';
if($action!='edit_form'){
	echo '<h3 OnClick="showhide(\'row_add\');" style="cursor: pointer; width: 180px;">'.si('add').' Добавить</h3><div id="row_add" style="';
	if($old_action!='add_row') echo 'display: none;';
	echo '">';
} else {
	echo '<h3>'.se('edit','','','',0).' Изменить</h3><div id="row_edit">';
}
echo '<a name="editform"></a>';
$dff='<form action="mod_table#form" method="post" enctype="multipart/form-data">'.$df2.'
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="id2" value="'.$id2.'">
	<input type="hidden" name="id6" value="'.$id6.'">
	<input type="hidden" name="id7" value="'.$id7.'">
	<input type="hidden" name="id3" value="'.$id3.'">';
if(isset($sort)) $dff.='<input type="hidden" name="sort" value="'.$sort.'">';

//по идее тут лучше этой проверки сделать очищение куки протекта при нажатии на кнопку "Сохранить", чтобы убить двух зайцев - защита от обновления после сохранения и допуск к возврату редактирования через кнопку "Назад"
if($action!='edit_form' && $GLOBALS["protect_admin_form"]) $dff.='<input type="hidden" name="form_protect" value="'.uuid().'"><input type="hidden" name="form_protect_type" value="1">';

echo $dff;
if($action!='edit_form') echo '<input type="hidden" name="action" value="add_row">';
else echo '<input type="hidden" name="action" value="edit_row">
		<input type="hidden" name="id4" value="'.$id4.'">
		<input type="hidden" name="id5" value="'.$id5.'">';


if(empty($id3)) $id3=0;
if($action!='edit_form') $id3=0;
$GLOBALS["cur_ex"]=$ex_ex2b;
$ex_ex2=$ex_ex2b;
echo input_form($table_id,$id,$id3,0,$ex_ex1,$ex_ex2);

if($table_multy){
	echo '<p><b>Родитель</b>:<br>';
	if($action!='edit_form'){
		//$rows=get_vars2(get_sub($id6,$table_id,0,1,0,0,1,$ex_ex1,$ex_ex2,$table_id,0,$table_id,1));//get_vars(get_sub($id6,$table_id,0,1,0,0,0,$ex_ex1,$ex_ex2));
		echo '<select name="ro_owner" id="ro_owner" OnChange="own_sel1(this);"><option value="0">Нет</option>';
		//options(/*$rows*/$rub_opt,' ',0,1);
		$opts=options($rub_opt,'',0,1,2);
		if(!empty($opts)) foreach($opts AS $opt){
			echo $opt->data;
		}
		echo '</select></p>';
	} else {
		//$rows=get_vars2(get_sub($id6,$table_id,0,1,0,0,1,$ex_ex1,$ex_ex2,$table_id,0,$table_id,1));//get_vars(get_sub($id6,$table_id,0,1,0,0,0,$ex_ex1,$ex_ex2));
		if(empty($id4) && !empty($id3) && !empty($id5)){
			$id4=getrowval("SELECT * FROM row_owner WHERE ro_ex=$ex_ex1 AND ro_id=$id5 AND row_id=$id3","owner_id");
			if(!empty($id4)) $id4=getrowval("SELECT * FROM row_owner WHERE ro_ex=$ex_ex1 AND row_id=$id4","ro_id");
		}
		echo '<select name="ro_owner" id="ro_owner" OnChange="own_sel1(this);"><option value="0">Нет</option>';
		//options(/*$rows*/$rub_opt,' ',$id4,2,1,0,$id3);
		$opts=options($rub_opt,'',$id4,2,2,0,$id3);
		if(!empty($opts)) foreach($opts AS $opt){
			echo $opt->data;
		}
		echo '</select></p>';
	}
} else echo '<input type="hidden" name="ro_owner" value="0">';

if(!empty($id6) && $action=='edit_form'){
	echo '<p><b>Глобальный родитель</b>:<br>';
	echo '<input type="hidden" id="ro_owner2d" value="'.$id6.'">';
	echo '<select name="ro_owner2" id="ro_owner2" OnChange="own_sel2(this);">';
	$opts=options($global_rows,'',$id6,3,2);
	if(!empty($opts)) foreach($opts AS $opt){
		echo $opt->data;
	}
	//options($global_rows,' ',$id6,3,1);
	echo '</select></p>';
}


$all_subtables=/*get_table_subtables_m2($tsubs_add);*/get_table_subtables_m($table_id,!$table_multy);
//тут нужна защита безопасности
$data='';
$checked=false;
if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value)/*if($var!=$id)*/{
	$last_cols_id=getall3($db,"SELECT col_id, col_table, col_module, col_inform FROM main_col WHERE col_table=0 AND col_module=$var AND col_inform=1","col_id");
	if(!empty($last_cols_id)){
		getrow($db,"SELECT value_table, value_row, value_col FROM row_value WHERE value_table=$table_id AND value_row=$id3 AND value_col IN (".implode(',',$last_cols_id).")");
		$check=(!empty($db->Record));
	} else $check=false;
	if($check) $checked=true;	
	if($check)	$z=input_form($table_id,$var,$id3,0,$ex_ex1,$ex_ex2,0,0,Array(),'',1,1,1);
	else $z='';
	if(!empty($last_cols_id)){
		if(!empty($tid)) $tid++; else $tid=1;
		if(!$z) $data.='<script>var tid'.$tid.'_loaded=false;</script>';
		else $data.='<script>var tid'.$tid.'_loaded=true;</script>';
		$ajax_id=$table_id.'!'.$var.'!'.$id3.'!'.$ex_ex1.'!'.$ex_ex2;
		$data.='<label style="cursor: pointer;"><h3 align="right">Перекрыть параметры подтаблицы «'.$value.'»<input type="checkbox" class="checkbox" name="subtable'.$var.'" OnChange="
			showhide(\'additional_table'.$tid.'\');
			if(!tid'.$tid.'_loaded){
				loadurlC(\'ex_overload_form\',\'additional_table'.$tid.'\',\''.$ajax_id.'\');
				tid'.$tid.'_loaded=true;
			}
		"'.($check?'checked':'').'></h3></label>';
		$data.='<div id="additional_table'.$tid.'" style="display: '.($check?'':'none').';">';
		$data.=$z;
		$data.='</div>';
	}
}

if(!empty($id3) && !empty($action) && $action=='edit_form'){

	//Блок настройки подтаблиц
	if(!empty($data)){
		echo '<div align="right"><span class="link" style="cursor: pointer;" OnClick="showhide(\'subtables-block\');">'.si('sub_tables').'Переопределить настройки для подтаблиц</span></div>';
		echo '<div id="subtables-block" style="display: '.($checked?'':'none').';">';
		echo $data;
		echo '</div>';
	}
	
	//Блок подтаблиц
	if(!empty($tsubs)) foreach($tsubs AS $stable_id=>$stable_name){
		$a=get_table_chields_count($id3,$stable_id);
		echo '<div align="right"><a href="'.$base_root.'?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id3.'&amp;id7='.$stable_id.'">'.si('point').$stable_name.'</a>'.$a.'</div>';
	}
	
	//Блок обзора статистики посещений объекта
	if(getrowval("SELECT visit_object FROM visit_object WHERE visit_object=$id3 AND visit_type=2 LIMIT 1",'visit_object')){
		$xday=date_to_xday();
		$xday_prev=$xday-1;
		$xmonth=date_to_xmonth();
		$xmonth_prev=$xmonth-1;
		$xyear=date_to_xyear();
		$xyear_prev=$xyear-1;
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=1 AND visit_time=$xday");
		$xday_host=$db->Record['host'];
		$xday_hit=$db->Record['hit'];
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=1 AND visit_time=$xday_prev");
		$xday_prev_host=$db->Record['host'];
		$xday_prev_hit=$db->Record['hit'];
		//getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=2 AND visit_time=$xmonth");
		$first_day_in_month=date_to_xday(date('Y'),date('m'),1);
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=1 AND visit_time>=$first_day_in_month");
		$xmonth_host=$db->Record['host'];
		$xmonth_hit=$db->Record['hit'];
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=2 AND visit_time=$xmonth_prev");
		$xmonth_prev_host=$db->Record['host'];
		$xmonth_prev_hit=$db->Record['hit'];
		//getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=3 AND visit_time=$xyear");
		$first_day_in_year=date_to_xmonth(date('Y'),1);
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=2 AND visit_time>=$first_day_in_year");
		$xyear_host=$db->Record['host']+$xmonth_host;
		$xyear_hit=$db->Record['hit']+$xmonth_hit;
		getrow($db,"SELECT SUM(visit_host) AS host, SUM(visit_hit) AS hit FROM visit_object WHERE visit_object=$id3 AND visit_type=2 AND visit_period=3 AND visit_time=$xyear_prev");
		$xyear_prev_host=$db->Record['host'];
		$xyear_prev_hit=$db->Record['hit'];
		echo '<div align="right" style="margin-top: 5px;"><span class="link" style="cursor: pointer;" OnClick="showhide(\'statistics-block\');">'.si('statistics').'Обзор посещаемости</span></div>';
		echo '<div id="statistics-block"  style="margin-top: 20px; margin-bottom: 20px; display: none;">';
		echo '<table id="records" cellpadding="3" cellspacing="1">
			<tr>
				<th>Период: </th>
				<th>За сегодня</th>
				<th>За вчера</th>
				<th>За этот месяц</th>
				<th>За предыдущий месяц</th>
				<th>За этот год</th>
				<th>За предыдущий год</th>
			</tr>
			<tr>
				<td>Визиты:</td>
				<td align="center" class="td-num">&nbsp;'.$xday_hit.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xday_prev_hit.'&nbsp;</td>			
				<td align="center" class="td-num">&nbsp;'.$xmonth_hit.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xmonth_prev_hit.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xyear_hit.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xyear_prev_hit.'&nbsp;</td>
			</tr>
			<tr>
				<td>Хосты:</td>
				<td align="center" class="td-num">&nbsp;'.$xday_host.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xday_prev_host.'&nbsp;</td>			
				<td align="center" class="td-num">&nbsp;'.$xmonth_host.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xmonth_prev_host.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xyear_host.'&nbsp;</td>
				<td align="center" class="td-num">&nbsp;'.$xyear_prev_host.'&nbsp;</td>
			</tr>
			</table>
			<div style="margin-top: 10px;">
				<a href="'.$zone_url.'/statistics?select_type=2&select_object='.$id3.'&period=1&select_day='.$xday.'&select_month='.$xmonth.'&select_year='.$xyear.'">Подробная статистика</a>
			</div>';
		echo '</div>';
	}
	
	// Блок связей
	$lcols=getall3($db,"SELECT col_id, col_type, col_link FROM main_col WHERE col_type=1 AND col_link=$table_id","col_id");
	if(!empty($lcols)){
		$nums=getall($db,"SELECT * FROM row_value WHERE value_value='$id3' AND value_col IN (".implode(',',$lcols).") LIMIT 500");
		if(!empty($nums)){
			$res='';
			$rcount=0;
			foreach($nums AS $num){
				$r='';
				if($num["value_table"]==0){
					$e_ex=$num["value_row"];
					if(check_ex($e_ex,'view',0)){
						$r_ex=getrow($db,"SELECT * FROM ex_module WHERE ex_id=$e_ex");
						if(!empty($r_ex)){
							if(check_ex($e_ex,'edit',0)) $r.='<a href="mod_main?id='.$r_ex["ex_module"].'&id2='.$e_ex.'&action=edit_ex_form#edit_ex">';
							$module_name=getrowval("SELECT module_name FROM main_module WHERE module_id=".$r_ex["ex_module"],'module_name');
							$r.='Экземпляр модуля "'.$module_name.'" ('.$r_ex["ex_name"].')"';
							if(check_ex($e_ex,'edit',0)) $r.='</a>';							
						}
					}
				} else {
					global $rlink;
					$row=$num["value_row"];
					$e_table=$num["value_table"];
					seek_rlink($row);
					if($rlink[$row]->enable){
						$e_ex=get_ex($row,$e_table,$rlink[$row]->tex);
						if(check_row($row,$e_table,$e_ex,"view",$rlink[$row]->user,$rlink[$row]->users)){
							$e_edit=check_row($row,$e_table,$e_ex,"edit",$rlink[$row]->user,$rlink[$row]->users);
							if($e_edit) $r.='<a href="'.get_admin_url($row).'">';
							$r.=get_basename($row,$e_table).' (таблица: '.getrowval("SELECT table_name FROM main_table WHERE table_id=$e_table","table_name").')';
							if($e_edit) $r.='</a>';
							
						}
					}
				}
				if($r) $res.='<div align="right" style="margin-bottom: 5px;">'.$r.' - </div>';
			}
			if(!empty($res)){
				echo '<div align="right" style="margin-top: 5px;"><span class="link" style="cursor: pointer;" OnClick="showhide(\'linked-block\');">'.si('arrow',5,0,'Ссылающиеся объекты').'Ссылающиеся объекты'.(count($nums)==500?' (первые 500)':'').'</span></div>';
				echo '<div id="linked-block" style="margin-bottom: 20px; margin-top: 20px; display: none;">';
				echo $res;
				echo '</div>';
			}
		}
	}
}


if($action=='edit_form') if(!empty($row_ids[$id3]) && count($row_ids[$id3])>1) echo '<p>Данная запись имеет несколько клонов (подчинений), при её изменении изменятся также все клоны (за исключением поля "родитель")</p>';

if($action!='edit_form'){
	echo '<input type="submit" class="button" value="Добавить"> или <span class="link" OnClick="$(\'#import\').toggle();">импортировать</span></form>'.$dff.'<div style="display: none;" id="import"><input type="hidden" name="action" value="import"><input type="file" name="data" class="button"> <input type="submit" class="button" value="Загрузить"></div>';
} else echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_table?id='.$id.'&amp;id2='.$id2.'&amp;id6='.$id6.'&amp;id7='.$id7.$df.'">вернуться назад</a>';
$GLOBALS["f_action"]='';

echo '</form>';

if($action!='edit_form'){
	// Загрузка объектов с сервера
	echo '<br>';
	if(!empty($GLOBALS['update_server']) && getrowval("SELECT module_date FROM main_module WHERE module_id=$id AND module_date!='0000-00-00 00:00:00' AND module_date!=''","module_date")){
		echo '<h3 OnClick="showhide(\'row_upload\'); $(\'#load_container\').load(jsq2+\'/ajax?action=get_remote_rows&m='.$module_id.'&t='.$table_id.'\');" style="cursor: pointer; width: 250px;">'.si('update').' Загрузить с сервера</h3><div id="row_upload" style="display: none;">';
		echo '<form action="mod_table" method="post" enctype="multipart/form-data">'.$df2;
		echo '<input type="hidden" name="id" value="'.$id.'">';
		echo '<input type="hidden" name="id2" value="'.$id2.'">';
		echo '<input type="hidden" name="id6" value="'.$id6.'">';
		echo '<input type="hidden" name="id7" value="'.$id7.'">';
		echo '<input type="hidden" name="id3" value="'.$id3.'">';		
		if(isset($sort)) $dff.='<input type="hidden" name="sort" value="'.$sort.'">';
		echo '<input type="hidden" name="action" value="upload">';
		
		echo '<br><div id="load_container">Ожидайте загрузки данных</div><br>';
		
		echo '<div><input type="submit" class="button" value="Загрузить"></div>';
		echo '</form></div>';
	}
}

echo '</div>';

$GLOBALS["f_type"]='bottom';
if($action!='edit_form' && !empty($table_bottom)){
	echo shell_tpl($table_bottom);
	$GLOBALS["exit"]=false;
	$GLOBALS["break"]=false;
	$GLOBALS["continue"]=false;
	$GLOBALS['break']=false;
	$GLOBALS['xbreak']=false;	
}
$GLOBALS["f_type"]='';

}


?>