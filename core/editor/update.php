<?php

@ini_set('max_execution_time',400);
error_reporting(E_ALL);
@ini_set('display_errors', 1);
if(!defined("DOCUMENT_ROOT")) define("DOCUMENT_ROOT", dirname(dirname(dirname(__FILE__))));
include_once(DOCUMENT_ROOT.'/core/units/constants.inc');
if(!defined('DEF_CHMOD')) define('DEF_CHMOD',0666);
if(!defined('DEF_DRMOD')) define('DEF_DRMOD',0777);

if(!empty($_GET["remote"])){//не везде работает ftp_chmod
	chmod(DOCUMENT_ROOT.'/core/config.inc',DEF_CHMOD);
}

if(!is_readable(DOCUMENT_ROOT.'/core/config.inc')) chmod(DOCUMENT_ROOT.'/core/config.inc',DEF_CHMOD);
include_once(DOCUMENT_ROOT.'/core/config.inc');
include_once(DOCUMENT_ROOT.'/core/units/db.inc');
include_once(DOCUMENT_ROOT.'/core/units/strings.inc');
include_once(DOCUMENT_ROOT.'/core/units/date.inc');
include_once(DOCUMENT_ROOT.'/core/units/files.inc');
include_once(DOCUMENT_ROOT.'/core/units/internet.inc');

include_once(DOCUMENT_ROOT.'/core/update/functions.inc');

if(!empty($_POST["action"])) $action=$_POST["action"];
if(!empty($_GET["action"])) $action=$_GET["action"];
if(!empty($_POST["inst"])) $inst=$_POST["inst"];
if(!empty($_GET["inst"])) $inst=$_GET["inst"];
if(!empty($action) && $action=='edit' && check_form_protection_key($_POST['key'],'update',1)){
	$GLOBALS["update_server"]=$_POST["upd_srv"];
	save_config();
}

function get_part_link_msg($parts,$prefix,$prefix2,$parts_data,$id){
	global $db;
	
	$st='';			
	$module_parts=unserialize($parts);
	$need_parts=Array();
	$need_parts2=Array();
	if(!empty($module_parts)) foreach($module_parts AS $type=>$needles)
			if(!empty($needles)) foreach($needles AS $part_sname)if(empty($need_parts[$part_sname]) && !empty($parts_data[$part_sname])){
				getrow($db,"SELECT part_sname, part_proc, part_type FROM main_part WHERE part_type=2 AND part_proc=$type AND part_sname='$part_sname'");
				if(empty($db->Record)){
					//тут тоже непонятно что писать... надо разбираться
					//$st.='<input type="hidden" name="ip['.$t[2].']" value="'.$btt.'">';
					$need_parts2[$part_sname]=$parts_data[$part_sname]['id'];
					$need_parts[$part_sname]=$parts_data[$part_sname]['name'];
				}
			}
	if(!empty($need_parts)){
		$st.='<div id="'.$prefix2.$id.'" style="display: none; padding: 8px; background-color: #F0F0F0; font-size: 11px;">Компоненты, необходимые для загрузки (будут загружены автоматически): ';
		$st.=implode(', ',$need_parts);
		$st.='<input type="hidden" name="'.$prefix.'['.$id.']" value="'.implode(',',$need_parts2).'">';
		$st.='</div>';
	}
	return $st;
}

if(!empty($action) && $action=='check2'){
	echo '<h2>Выберите модули для загрузки</h2>';
	echo '<div><i>Не забудьте обновить файлы системы перед обновлением модулей</i></div>';
	$have_upload=false;
	$have_update=false;
	$out='';
	$tmp=getall($db,"SELECT * FROM main_module",1,"main_module");
	$m=Array();
	foreach($tmp AS $tm) $m[$tm["module_sname"]]=$tm;
	$tmp=explode('???',loadserv($GLOBALS["update_server"].'?type=mod'));
	if(empty($tmp[0])){
		$tmp=Array();
		$tm=Array();
		echo '<div><b>Сервер обновлений не работает</b></div>';
	} else {
		$tm=explode('///',$tmp[0]);
	}
	$st='';
	$m2=Array();
	foreach($tm AS $t)if($t!=''){
		$t=explode('|||',$t);
		if(empty($m[$t[1]])) $m2[$t[2]]=$t[0];
	}
	$parts_data=Array();
	if(!empty($tmp[2])){
		$parts_data_tmp=explode('///',$tmp[2]);
		foreach($parts_data_tmp AS $pdt){
			$t=explode('|||',$pdt);
			$parts_data[$t[2]]['id']=$t[4];
			$parts_data[$t[2]]['name']='<b>'.$t[1].'</b>';
		}
	}
	foreach($tm AS $t)if($t!=''){
		$t=explode('|||',$t);
		if(empty($m[$t[1]])){
			$st.='<div>';
			$st.='<label style="cursor: pointer;"><input type="checkbox" name="m2['.$t[2].']" class="button" OnClick="showhide(\'hs'.$t[2].'\');showhide(\'hsp'.$t[2].'\');"> '.$t[0].'</label>';
			$btt=Array();
			$ats=explode(',',$t[3]);
			if(!empty($ats)) for($i=0;$i<count($ats);$i++) if(!empty($m2[$ats[$i]])) $btt[]=$ats[$i];
			$btt=implode(',',$btt);
			$st.='<input type="hidden" name="i['.$t[2].']" value="'.$btt.'">';
			$first=true;
			if(!empty($ats)) foreach($ats AS $at)if(!empty($m2[$at])){
				if($first){
					$st.='<div id="hs'.$t[2].'" style="display: none; padding: 8px; background-color: #F0F0F0; font-size: 11px;">Модули, необходимые для загрузки (будут загружены автоматически): ';
				}
				if(!$first) $st.=', ';
				$st.='<b>'.$m2[$at].'</b>';
				$first=false;
			}
			if(!$first) $st.='</div>';
			
			if(!empty($t[4])) $st.=get_part_link_msg($t[4],'m2p','hsp',$parts_data,$t[2]);
			
			$st.='</div>';
		}
	}
	
	// Список модулей
	
	if(!empty($st)) $out.='<br><div><b>Модули</b></div>'.$st;

	$ta=getall($db,"SELECT * FROM part_cat",1,"part_cat");
	$ct=Array();
	foreach($ta AS $t) $ct[$t["cat_type"].$t["cat_name"]]=$t;
	if(!empty($tmp[1])){
		$tm=explode('///',$tmp[1]);
	} else $tm=Array();
	foreach($tm AS $t){
		$t=explode('|||',$t);
		$part_cat[$t[1]]=$t[0];
		$part_cat2[$t[1]]=$t[2];
		$part_cat3[$t[1]]=$t[4];
	}
	
	// Список компонентов для загрузки (и групп компонентов)

	$tm2=getall($db,"SELECT part_id, part_name, part_type, part_proc, part_sname, part_uin, part_date, part_date2, part_cat FROM main_part WHERE part_type=2",1,"main_part");
	$p=Array();
	if(!empty($tm2)) foreach($tm2 AS $tm) $p[$tm["part_proc"]][$tm["part_sname"]]=$tm;
	if(!empty($tmp[2])) $tm=explode('///',$tmp[2]); else $tm=Array();
	$st=Array();
	$st[0]='';$st[1]='';$st[2]='';$st[3]='';
	$type2[0]='функций';
	$type2[1]='отображений';
	$type2[2]='компонентов';
	$type2[3]='форм';
	$update_parts=Array();
	foreach($tm AS $t){
		$t=explode('|||',$t);
		$update_parts[$t[0].'.'.$t[2]]['date']=$t[6];
		$update_parts[$t[0].'.'.$t[2]]['id']=$t[4];
		$update_parts[$t[0].'.'.$t[2]]['parts']=$t[7];
		$cat=$t[3];
		if($cat!=0){
			$type=$part_cat2[$t[3]];
			$name=$part_cat[$t[3]];
			$cat_parts=$part_cat3[$t[3]];
		} else $name='';//?
		/*if($cat!=0 && empty($ct[$type.$name])){
			if(empty($ag[$cat])){
				$ag[$cat]=1;
				$st[$t[0]].='<div><label style="cursor: pointer;"><input type="checkbox" name="pg['.$cat.']" class="button" OnClick="showhide(\'csp'.$t[3].'\');"> Группа '.$type2[$type].' «'.$name.'»</label></div>';
				if(!empty($cat_parts)) $st[$t[0]].=get_part_link_msg($cat_parts,'cat2p','csp',$parts_data,$t[3]);
			}
		} else if(empty($p[$t[0]][$t[2]])){
			//$st[$t[0]].='<div><input type="checkbox" name="p['.$t[0].']['.$t[2].']" class="button"> '.$t[1].'</div>';
			$st[$t[0]].='<div><label style="cursor: pointer;"><input type="checkbox" name="p['.$t[4].']" class="button" OnClick="showhide(\'psp'.$t[4].'\');"> '.$t[1].'</label></div>';
			if(!empty($t[7])) $st[$t[0]].=get_part_link_msg($t[7],'p2p','psp',$parts_data,$t[4]);
		}*/
		
		if(empty($p[$t[0]][$t[2]])){
			if(empty($st[$t[0]][$name])) $st[$t[0]][$name]='';
			$st[$t[0]][$name].='<div><label style="cursor: pointer;"><input type="checkbox" name="p['.$t[4].']" class="button upload_set" OnClick="showhide(\'psp'.$t[4].'\');"> '.$t[1].'</label></div>';
			if(!empty($t[7])) $st[$t[0]][$name].=get_part_link_msg($t[7],'p2p','psp',$parts_data,$t[4]);
			$have_upload=true;
		}
		
	}
	for($i=0;$i<4;$i++){
		if(!empty($st[$i])){
			$out.='<br><div><b>';
			if($i==0) $out.='Функции';
			if($i==1) $out.='Отображения';
			if($i==2) $out.='Компоненты';
			if($i==3) $out.='Формы';
			$out.='</b></div><div class="box">';
			$f=true;
			foreach($st[$i] AS $cat_name=>$data){
				if($f) $out.='<div style="margin-top: 5px; margin-bottom: 5px;"><b>'.$cat_name.'</b></div>';
				$out.=$data;
			}
			$out.='</div>';
		}
	}
	
	// Шаблоны таблиц
	if(!empty($tmp[3])) $tm3=explode('///',$tmp[3]); else $tm3=Array();
	$tbltpl=getall4($db,"SELECT * FROM main_table WHERE table_module=0","table_uin");
	$u_tbltpl=Array();
	$first=true;
	if(!empty($tm3)) foreach($tm3 AS $table){
		$table=explode('|||',$table);
		$table_id=$table[2];
		$table_name=$table[0];
		$table_uin=$table[3];
		$table_date2=$table[4];
		$table_parts=$table[5];
		if(!empty($tbltpl[$table_uin]) && (empty($tbltpl[$table_uin]['table_date']) || $tbltpl[$table_uin]['table_date']<$table_date2)){
			$u_tbltpl[$table_id]=Array('name'=>$table_name,'parts'=>$table_parts,'uin'=>$tbltpl[$table_uin]['table_uin']);
		} else if(empty($tbltpl[$table_uin])){
			if($first) $out.='<br><div><b>Загрузка шаблонов таблиц</b></div>';
			$first=false;
			$out.='<div><label style="cursor: pointer;"><input type="checkbox" name="tbltpl['.$table_id.']" class="button upload_set" OnClick="showhide(\'tsp'.$table_id.'\');"> '.$table_name.'</label></div>';
			if(!empty($table_parts)) $out.=get_part_link_msg($table_parts,'table2p','tsp',$parts_data,$table_id);
			$have_upload=true;
		}
	}
	if(!empty($u_tbltpl)){
		$out.='<br><div><b>Обновить шаблоны таблиц</b></div>';
		foreach($u_tbltpl AS $table_id=>$table_array){
			$out.='<div><label style="cursor: pointer;"><input type="checkbox" name="u_tbltpl['.$table_id.']" class="button update_set" OnClick="showhide(\'tsp'.$table_id.'\');"> '.$table_array['name'].'</label></div>';
			$out.='<input type="hidden" name="tbltpl_uin['.$table_id.']" value="'.$table_array['uin'].'">';
			if(!empty($table_array['parts'])) $out.=get_part_link_msg($table_array['parts'],'table2p','tsp',$parts_data,$table_id);			
			$have_update=true;
		}
	}
	
	// Шаблоны полей
	if(!empty($tmp[4])) $tm4=explode('///',$tmp[4]); else $tm4=Array();
	$coltpl=getall4($db,"SELECT * FROM main_col WHERE col_module=0 AND col_table=0","col_uin");
	$u_coltpl=Array();
	$first=true;
	if(!empty($tm4)) foreach($tm4 AS $col){
		$col=explode('|||',$col);
		$col_id=$col[2];
		$col_name=$col[0];
		$col_uin=$col[3];
		$col_date2=$col[4];
		$col_parts=$col[5];
		if(!empty($coltpl[$col_uin]) && (empty($coltpl[$col_uin]['col_date']) || $coltpl[$col_uin]['col_date']<$col_date2)){
			$u_coltpl[$col_id]=Array('name'=>$col_name,'parts'=>$col_parts,'uin'=>$coltpl[$col_uin]['col_uin']);
		} else if(empty($coltpl[$col_uin])){
			if($first) $out.='<br><div><b>Загрузка шаблонов переменных</b></div>';
			$first=false;
			$out.='<div><label style="cursor: pointer;"><input type="checkbox" name="coltpl['.$col_id.']" class="button upload_set" OnClick="showhide(\'csp'.$col_id.'\');"> '.$col_name.'</label></div>';
			if(!empty($col_parts)) $out.=get_part_link_msg($col_parts,'col2p','csp',$parts_data,$col_id);
			$have_upload=true;
		}
	}
	if(!empty($u_coltpl)){
		$out.='<br><div><b>Обновить шаблоны переменных</b></div>';
		foreach($u_coltpl AS $col_id=>$col_array){
			$out.='<div><label style="cursor: pointer;"><input type="checkbox" name="u_coltpl['.$col_id.']" class="button update_set" OnClick="showhide(\'csp'.$col_id.'\');"> '.$col_array['name'].'</label></div>';
			$out.='<input type="hidden" name="coltpl_uin['.$col_id.']" value="'.$col_array['uin'].'">';
			if(!empty($col_array['parts'])) $out.=get_part_link_msg($col_array['parts'],'col2p','csp',$parts_data,$col_id);
			$have_update=true;
		}
	}
	
	// Список компонентов для обновления
	$up_parts=Array();
	if(!empty($tm2)) foreach($tm2 AS $tm){
		$last_update_date=$tm["part_date"];
		$lkey=$tm["part_proc"].'.'.$tm["part_sname"];
		if(empty($update_parts[$lkey])) continue;
		$current_date=$update_parts[$lkey]['date'];
		if($current_date>$last_update_date){
			//echo '<br><b>'.$tm['part_name'].'</b><br>date on client: '.$last_update_date.', on server: '.$current_date.'<br><Br>';
			$key=$tm["part_id"];
			$up_parts[$key]['name']=$tm["part_name"];
			$up_parts[$key]['id']=$tm["part_id"].';'.$update_parts[$lkey]['id'];
		}
	}
	if(!empty($up_parts)){
		$out.='<br><div><b>Обновление компонентов</b></div>';
		foreach($up_parts AS $part){
			$out.='<div><label style="cursor: pointer;"><input type="checkbox" name="up['.$part['id'].']" class="button update_set" OnClick="showhide(\'psp'.$part['id'].'\');"> '.$part['name'].'</label></div>';
			if(!empty($part['parts'])) $out.=get_part_link_msg($part['parts'],'p2p','psp',$parts_data,$part['id']);
			$have_update=true;
		}
	}	

	if(!empty($out)){
		if($have_upload) echo '<p><span class="link" OnClick="$(\'.upload_set\').each(function(){$(this).attr(\'checked\',true);});">Выбрать все загрузки</span></p>';
		if($have_update) echo '<p><span class="link" OnClick="$(\'.update_set\').each(function(){$(this).attr(\'checked\',true);});">Выбрать все обновления</span></p>';
		echo '<form action="update?action=update2" method="post">
					<input type="hidden" name="form_protect" value="'.uuid().'">
					'.$out.'
					<input type="submit" value="Загрузить" class="button">
			</form>';
	} else echo '<div><b>Обновление не найдено</b></div>';
	echo '<br><br>';
}
if(!empty($action) && $action=='check'){
	$temp=ob_get_contents();
	if($temp) ob_end_clean();
	ob_start();
	error_reporting(REPORTING_LEVEL);
	if(!function_exists('gzdecode'))	$_GET["nogzip"]=1;
	include(DOCUMENT_ROOT.'/core/update/server.php');
	$temp2=ob_get_contents();
	ob_end_clean();
	if($temp){
		ob_start();
		echo $temp;
	}
	error_reporting(REPORTING_LEVEL);
	@$ups=loadserv($GLOBALS["update_server"]);
	if(empty($ups)){
		echo '<div>Сервер недоступен</div>';
	} else {
	$src2=$ups;
	$src2=explode('!-!-!',$src2);
	$src_sql=$src2[0];
	if(!empty($src2[1]))$src2=$src2[1]; else $src2='';
	$temp2=gzdecode1($temp2);
	$temp2=explode('!-!-!',$temp2);
	$dst_sql=$temp2[0];
	$temp2=$temp2[1];
	$dst2=explode('|',$temp2);
	$src2=explode('|',$src2);
	$src=Array();$dst=Array();
	//сравнения файлов
	foreach($src2 AS $sr)if(!empty($sr) && strpos($sr,'=')){$tmp=explode('=',$sr); $src[$tmp[0]]=$tmp[1];}
	foreach($dst2 AS $ds)if(!empty($ds) && strpos($ds,'=')){$tmp=explode('=',$ds); $dst[$tmp[0]]=$tmp[1];}
	$ts=0; $diff=Array(); $diff2=Array();
	foreach($src AS $var=>$value){	
		if(empty($dst[$var])){
			if($value=='-') $value=1;
			$ts+=$value; $diff2[]=$var;
		} else if($dst[$var]!=$value){
			$diff[]=$var;
			$ts+=$value;
		}
	}
	//сравнение баз данных
	//подсчёт количества таблиц
	$src_c1=count(explode('!-',$src_sql));
	$src_c2=count(explode('!.',$src_sql));
	$dst_c1=count(explode('!-',$dst_sql));
	$dst_c2=count(explode('!.',$dst_sql));
	
	//сравнение столбцов (без привязки к таблице)
	$difcol=0;
	foreach(explode('!.',$dst_sql) AS $val){
		$val=get_tag($val,'Field!=','!;');
		$d1[$val]=1;
	}
	foreach(explode('!.',$src_sql) AS $val){
		$val=get_tag($val,'Field!=','!;');
		if(empty($d1[$val])) $difcol++;
	}
	
	// проверка на обновлённые индексы
	$tables_src=explode('!-',$src_sql);
	$tables_dst=explode('!-',$dst_sql);
	foreach($tables_src AS $tbl){
		$tmp=explode('!:',$tbl);
		$table_sname=$tmp[0];
		$indexes=explode('!;',$tmp[2]);
		//$ind_src[$table_sname]=Array();
		if(!empty($indexes)) foreach($indexes AS $index) if(!empty($index)){
			$index=explode('!=',$index);
			$ind_src[$table_sname][$index[0]]=$index[1];
			//echo $table_sname.' - '.$index[0].' - '.$index[1].'<br>';
		}
	}
	//echo '<br><br><Br><br><br>';
	foreach($tables_dst AS $tbl){
		$tmp=explode('!:',$tbl);
		$table_sname=$tmp[0];
		$indexes=explode('!;',$tmp[2]);
		//$ind_dst[$table_sname]=Array();
		if(!empty($indexes)) foreach($indexes AS $index) if(!empty($index)){
			$index=explode('!=',$index);
			$ind_dst[$table_sname][$index[0]]=$index[1];
			//echo $table_sname.' - '.$index[0].' - '.$index[1].'<br>';
		}
	}
	$difind=0;
	if(!empty($ind_src)) foreach($ind_src AS $table_sname=>$tmp) foreach($tmp AS $ind_name=>$ind_cols){
		/*if(!isset($ind_dst[$table_sname])){
			$difind++;
			$ts+=20*1024;
			//echo 'Table not exist: '.$table_sname.'<br>';
			break;
		}*/
		if(empty($ind_dst[$table_sname][$ind_name]) || $ind_dst[$table_sname][$ind_name]!=$ind_cols){
			$difind++;
			//echo $table_sname.' -> '.$ind_name.' = '.$ind_cols.'<br>';
			$ts+=20*1024;
			//if(empty($ind_dst[$table_sname][$ind_name])) echo 'Index does not exist: '.$ind_name.'<br>';
			//else if($ind_dst[$table_sname][$ind_name]!=$ind_cols) echo 'Index does not match: '.$ind_name.' ('.$ind_dst[$table_sname][$ind_name].' != '.$ind_cols.')<br>';
			continue;
		}
	}
	
	/*foreach(explode('!.',$dst_sql) AS $val){
		$g1[$val]=1;
	}
	foreach(explode('!.',$src_sql) AS $val){
		if(empty($g1[$val])) echo $val.'<br>';
		$g2[$val]=1;
	}
	echo '<br>';
	foreach(explode('!.',$dst_sql) AS $val){
		if(empty($g2[$val])) echo $val.'<br>';
	}
	echo $src_c1.' = '.$dst_c1.'<br>'.$src_c2.'='.$dst_c2.'<br>';*/
	if($src_c1!=$dst_c1 || $difcol!=0/*$src_c2!=$dst_c2*/){
		$ts+=20*1024;
	}
	if(round($ts/1024)==0) echo '<p><b>Обновлений не найдено</b></p><br><br>';
	else {
		echo '<p><b>Найдено обновление</b>. <a href="update?action=update">Обновить</a> ('.round($ts/1024).'кб). <span OnClick="showhide(\'upd\');" class="link">'.si('view').'</span></p>';	
		echo '<div style="display: none;" id="upd">';
		$d=false;
		if($difcol>0){ $d=true; echo '<div>Сущностей БД к обновлению: '.$difcol.'</div>'; }
		if($difind>0){ $d=true; echo '<div>Добавление индексов: '.$difind.'</div>'; }
		if($src_c1!=$dst_c1){ $d=true; echo '<div>Добавление новых таблиц в БД</div>'; }
		if(!empty($diff)) foreach($diff AS $val) {$d=true; echo '<div>Файл: '.$val.' (обновление)</div>'; }
		if(!empty($diff2)) foreach($diff2 AS $val) {$d=true; echo '<div>Файл: '.$val.' (новый)</div>'; }
		if(!$d) echo 'Невозможно определить объекты обновления';
		echo '</div>';
		if($difind>0){
			echo '<br><b>Внимание</b>! При обновлении будут добавлены новые индексы. Это может занять длительное время. Если страница выдала ошибку, подождите около 5 минут, затем вновь зайдите в обновления и повторите операцию.';
		}
		echo '<br><br>';
	}
	}
}
if(!empty($action) && $action=='update'){
	if(empty($inst)) deltemp(); //в т.ч. удаляет все json файлы редактора, что делает лишним наличие reset_cmd_json();
	$temp=ob_get_contents();
	if($temp) ob_end_clean();
	ob_start();
	error_reporting(REPORTING_LEVEL);
	if(!function_exists('gzdecode'))	$_GET["nogzip"]=1;
	include(DOCUMENT_ROOT.'/core/update/server.php');
	$temp2=gzdecode1(ob_get_contents());
	echo DOCUMENT_ROOT.'/core/update/server.php';
	ob_end_clean();
	if($temp){
		ob_start();
		echo $temp;
	}
	error_reporting(REPORTING_LEVEL);

	$tt='';
	//if(isset($_GET["remote"])) $tt.='?remote='.$_GET["remote"];
	@$src2=loadserv($GLOBALS["update_server"].$tt);
	if(empty($src2)){
		echo '<p><b style="color: #FF0000;">Ошибка!</b><br>Сервер обновлений недоступен</p>';
	} else {
		$src2=explode('!-!-!',$src2);
		$src_sql=$src2[0];
		$src2=$src2[1];
		$temp2=explode('!-!-!',$temp2);
		$dst_sql=$temp2[0];

		function prepend_sql($str){
			$dts=explode('!-',$str);
			$tbl=Array();$i=0;
			foreach($dts AS $dt){
				$dt=explode('!:',$dt);
				$i=$dt[0];
				$dt=explode('!.',$dt[1]);
				foreach($dt AS $d){
					$fs=explode('!;',$d);
					$tmp=Array();
					foreach($fs AS $f){
						$f=explode('!=',$f);
						$tmp[$f[0]]=$f[1];
					}
					if(isset($tmp["Field"])) $tbl[$i][$tmp["Field"]]=$tmp;
					else {
						//foreach($tmp AS $v=>$v2) echo $v.'='.$v2.'<br>';
						//echo '<br><Br>';
					}
				}
			}
			return $tbl;
		}
		$dst_tbls=prepend_sql($dst_sql);	//дст - текущий, срц - с сервера обновлений
		$src_tbls=prepend_sql($src_sql);
		$line=0;//debug var
		foreach($src_tbls AS $tname=>$src_tbl){
			if(empty($dst_tbls[$tname])){
				$sql_e='';
				$sql='CREATE TABLE '.$tname.' (';
				$first=true;
				foreach($src_tbls[$tname] AS $field=>$val){
					if(!$first) $sql.= ',';$first=false;
					$sql.= '`'.$val["Field"].'` '.$val["Type"].' ';
					if(!empty($val["Null"])) $sql.=$val["Null"].' ';
					if(isset($val["Default"]) && $val["Default"]!=''){
						//if($val["Type"]=='text') $sql.="DEFAULT '".$val["Default"]."'";
						//else $sql.="DEFAULT ".$val["Default"];
						$sql.="DEFAULT ".$val["Default"];
					}
					if(!empty($val["Extra"])) $sql.=' '.$val["Extra"];
					if(!empty($val["Key"]) && $val["Key"]=='PRI') $sql_e.=', PRIMARY KEY(`'.$val["Field"].'`)';
					if(!empty($val["Key"]) && $val["Key"]!='PRI') $sql_e.=', KEY(`'.$val["Field"].'`)';
					if(!empty($val["Index"]) && $val["Index"]!='PRI') $sql_e.=', INDEX(`'.$val["Field"].'`)';
				}
				$sql.=$sql_e;
				$sql.=') ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;';
				global $db;
				$db->query($sql);
			} else foreach($src_tbl AS $cname=>$src_col){
				$line++;
				//echo $tname.' -> '.$line.'. '.$cname.'<br>';
				if(!isset($dst_tbls[$tname][$cname])){
					$sql='ALTER TABLE '.$tname.' ADD ';
					$val=$src_tbls[$tname][$cname];
					$sql.= '`'.$val["Field"].'` '.$val["Type"].' ';
					if(!empty($val["Null"])) $sql.=$val["Null"].' ';
					if(!empty($val["Default"])){
						//$sql.="DEFAULT '".$val["Default"]."'";
						$sql.="DEFAULT ".$val["Default"];
					}
					if(!empty($val["Extra"])) $sql.=' '.$val["Extra"];
					global $db;
					$db->query($sql);
				}
			}
		}

		$temp2=$temp2[1];
		$dst2=explode('|',$temp2);
		$src2=explode('|',$src2);
		$src=Array();$dst=Array();
		foreach($src2 AS $sr)if(!empty($sr) && strpos($sr,'=')){$tmp=explode('=',$sr); $src[$tmp[0]]=$tmp[1];}
		foreach($dst2 AS $ds)if(!empty($ds) && strpos($ds,'=')){$tmp=explode('=',$ds); $dst[$tmp[0]]=$tmp[1];}
		global $no_update;
		foreach($src AS $var=>$value){
			if(empty($dst[$var]) || $dst[$var]!=$value){
				if($value=='-'){
					@mkdir(DOCUMENT_ROOT.'/'.$var);
					@chmod(DOCUMENT_ROOT.'/'.$var,DEF_DRMOD);
				} else {
					update_file($var);
				}
			}
		}
	
		if(!empty($inst)){
			global $db;
			if(!file_exists(DOCUMENT_ROOT.'/.htaccess')){
				$f=fopen(DOCUMENT_ROOT.'/.htaccess','w');
				fwrite($f,BASE_HTACCESS);
				fclose($f);
			}
			$db->query("INSERT INTO `main_zone` (`zone_id`, `zone_active`, `zone_domain`, `zone_folder`, `zone_email`, `zone_redirect`, `zone_module`, `zone_name`) VALUES (1, 1, '', '*admin', '', 0, '-1', 'Control')");
			$db->query("INSERT INTO `auth_perm` (`perm_id`, `perm_target`, `perm_type`, `perm_auth`, `perm_object`, `perm_folder`, `perm_view`, `perm_edit`, `perm_add`, `perm_del`, `perm_control`, `perm_rules`, `perm_upload`, `perm_maxupl`, `perm_reg`, `perm_invite`, `perm_leave`, `perm_unreg`) VALUES (1, 1, 0, 0, 1, '', 0, -1, -1, -1, -1, -1, -1, 0, -1, -1, -1, -1)");
			true_unicode();
		}
		
		// Добавление новых индексов
		$tables_src=explode('!-',$src_sql);
		$tables_dst=explode('!-',$dst_sql);
		foreach($tables_src AS $tbl){
			$tmp=explode('!:',$tbl);
			$table_sname=$tmp[0];
			$indexes=explode('!;',$tmp[2]);
			if(!empty($indexes)) foreach($indexes AS $index) if(!empty($index)){
				$index=explode('!=',$index);
				$ind_src[$table_sname][$index[0]]=$index[1];
			}
		}
		foreach($tables_dst AS $tbl){
			$tmp=explode('!:',$tbl);
			$table_sname=$tmp[0];
			$indexes=explode('!;',$tmp[2]);
			if(!empty($indexes)) foreach($indexes AS $index) if(!empty($index)){
				$index=explode('!=',$index);
				$ind_dst[$table_sname][$index[0]]=$index[1];
			}
		}
		$difind=0;
		if(!empty($ind_src)) foreach($ind_src AS $table_sname=>$tmp) foreach($tmp AS $ind_name=>$ind_cols){
			//if(empty($ind_dst[$table_sname])) break;
			if(empty($ind_dst[$table_sname][$ind_name]) || $ind_dst[$table_sname][$ind_name]!=$ind_cols){
				if(!empty($ind_dst[$table_sname][$ind_name])) $db->query("ALTER TABLE $table_sname DROP INDEX $ind_name");
				$db->query("ALTER TABLE $table_sname ADD INDEX $ind_name ($ind_cols)");
			}
		}
		
		echo '<p><b>Обновление завершено</b></p><br><br>';
	}
}

if(!empty($action) && $action=='manual_update'){
	update_version();
}

if(!empty($action) && $action=='update2'){
	//p - part_id, pg - cat_id, m - module_id, up - update parts
	
	// Обновление частей
	if(!empty($up)) foreach($up AS $part_sid=>$value){
		if(!empty($_POST["p2p"][$part_sid])){
			$op=explode(',',$_POST["p2p"][$part_sid]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
		$part_sid=explode(';',$part_sid);
		$this_part_id=$part_sid[0];//id этой части на локальном сервере
		$server_part_id=$part_sid[1];//id этой части на сервере обновлении
		//$params=getall3($db,"SELECT * FROM part_param WHERE param_part=$this_part_id","param_id");//?
		
		import_part($server_part_id,0,'',0,$this_part_id);
		//можно через удаление и загрузку части, но нужно не забыть переписать связи с параметрами модуля
		//update_part($part_id,0);
		//main_news -> news_part
		//main_con -> col_paramlink
	}

	$p2=Array();	
	if(!empty($p)) foreach($p AS $var=>$value){
		if(!empty($_POST["p2p"][$var])){
			$op=explode(',',$_POST["p2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
		import_part($var,0);		
	}
	if(!empty($pg)) foreach($pg AS $var=>$value){
		if(!empty($_POST["cat2p"][$var])){
			$op=explode(',',$_POST["cat2p"][$var]);			
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
		import_cat($var);
	}
	$nm=Array();
	if(!empty($_POST["m2"])) $m2=$_POST["m2"]; else $m2=Array();
	if(!empty($m2)) foreach($m2 AS $var=>$value)if(!empty($value)){	
		// Идентификация недостающих частей
		if(!empty($_POST["m2p"][$var])){
			$op=explode(',',$_POST["m2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
		// Подключение связанных модулей
		if(!empty($_POST['i'][$var])){
			$ts=explode(',',$_POST['i'][$var]);foreach($ts AS $t) $nm[$t]=1;
		}
		$nm[$var]=1;
	}
	
	// Загрузка шаблонов таблиц
	if(!empty($_POST['tbltpl'])) foreach($_POST['tbltpl'] AS $var=>$value){
		$o=prepend_mod_install();
		import_table($var,'load');
		after_mod_install($o);
		if(!empty($_POST["table2p"][$var])){
			$op=explode(',',$_POST["table2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
	}
	
	// Обновление шаблонов таблиц
	if(!empty($_POST['u_tbltpl'])) foreach($_POST['u_tbltpl'] AS $var=>$value){
		del_table(getrowval("SELECT table_id FROM main_table WHERE table_module=0 AND table_uin='".$_POST['tbltpl_uin'][$var]."'",'table_id'));
		$o=prepend_mod_install();
		import_table($var,'load');
		after_mod_install($o);
		if(!empty($_POST["table2p"][$var])){
			$op=explode(',',$_POST["table2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
	}
	
	// Загрузка шаблонов полей
	if(!empty($_POST['coltpl'])) foreach($_POST['coltpl'] AS $var=>$value){
		import_col($var,0,'load');
		if(!empty($_POST["col2p"][$var])){
			$op=explode(',',$_POST["col2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
	}
	
	// Обновление шаблонов полей
	if(!empty($_POST['u_coltpl'])) foreach($_POST['u_coltpl'] AS $var=>$value){
		$acol=getrowval("SELECT col_id FROM main_col WHERE col_module=0 AND col_table=0 AND col_uin='".$_POST['coltpl_uin'][$var]."'",'col_id');
		if(empty($acol)) continue;
		del_col($acol);
		import_col($var,0,'load');
		if(!empty($_POST["col2p"][$var])){
			$op=explode(',',$_POST["col2p"][$var]);
			foreach($op AS $opp) $p2[$opp]=$opp;
		}
	}
	
	//Догрузка недостающих частей
	if(!empty($p2)) foreach($p2 AS $var=>$value) import_part($var,0);
	
	//if(!empty($nm)) foreach($nm AS $var=>$value) echo $var.'<Br>';
	//exit;
	$o=prepend_mod_install();
	if(!empty($nm)) foreach($nm AS $var=>$value){
		if(is_numeric($var)) import_module($var);
		else import_module(0,$var);
		reset_module_json(0,'',$var);
	}
	after_mod_install($o);
	
	after_mod_install();//финальные сравнения для обновлённых частей и т.д.
	echo '<p><b>Обновление завершено</b></p><br><br>';
	echo '<div>Внимание! Если вместе с модулем вы загрузили группы пользователей, то установите в них доступ к зонам (например к зоне управления) и доступы к папкам (включая родительский каталог) САМОСТОЯТЕЛЬНО.<br>Такое требование связано с тем, что структура папок и зоны могут отличаються на вашей копии системы и на сервере обновлений</div>';
	reset_components_json();
}

echo '<form action="update" method="post"><input type="hidden" name="action" value="edit"><p><b>Сервер обновлений</b>:<br><input type="text" name="upd_srv" value="'.$GLOBALS["update_server"].'"><input type="submit" class="button" value="Изменить"></p>'.get_form_protection_key('update',1,1).'</form>';
if(check_update_requirement()) echo '<p><a href="update?action=manual_update" style="font-size: 18px;">Выполнить сервисные операции, для адаптации системы к последнему обновлению</a></p>';
echo '<p><a href="update?action=check">Проверить обновление</a></p>';
echo '<p><a href="update?action=check2">Загрузить новые модули и компоненты</a></p>';
echo '<p>Внимание!<br>Не загружайте сразу много компонентов. Ваш сервер может не успеть получить всю информацию за время жизни скрипта. Если ваш сервер выдал ошибку в процессе выполнения (NGIX, Timeout), то не спешите обновлять повторно. Подождите пару минут - вполне возможно, что за это время файлы догрузятся в фоновом режиме</p>';

?>