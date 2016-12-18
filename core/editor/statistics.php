<?php

define_lng('statistics');

if(empty($GLOBALS["senter"])){
	include('main.php'); exit;
}

if($user->super) $add_sql='';
else {
	$zallow=Array();
	foreach($zone AS $z) if(check_zone($z["zone_id"],'rules')) $zallow[$z["zone_id"]]=$z["zone_id"];
	$add_sql=' AND visit_object IN ('.implode(',',$zallow).')';
}

// функции

function getall_statistics($query,$res=Array(),$time_override=0,$save_time=0){
	global $db;
	$times=getall($db,$query);
	$thi=Array();
	$tho=Array();
	foreach($times AS $time){
		$obj=$time["visit_object"];
		$t=$time["visit_time"];
		$hit=$time["visit_hit"];
		$host=$time["visit_host"];
		if($time_override!=0) $t=$time_override;
		$res[$obj]->time[$t]->hit=$hit;
		$res[$obj]->time[$t]->host=$host;
		if(!$time_override){
			if(empty($thi[$obj])) $thi[$obj]=$hit; else $thi[$obj]=$thi[$obj]+$hit;
			$res[$obj]->time[$t]->total_hit+=$thi[$obj];
			if(empty($tho[$obj])) $tho[$obj]=$host; else $tho[$obj]=$tho[$obj]+$host;
			$res[$obj]->time[$t]->total_host+=$tho[$obj];
		} else {
			
		}
		if($save_time) $res[$obj]->last_t=$t; // нужно только для определения месяца при отображении триггеров за 14 дней
		$res[$obj]->total_hit+=$hit;
		$res[$obj]->total_host+=$host;
	}
	return $res;
}

function getall_statistics2($query){
	global $db;
	$objects=getall($db,$query);
	$res->total_hit=0;
	$res->total_host=0;
	$res->t=Array();
	foreach($objects AS $object){
		$t=$object["visit_time"];
		$hit=$object["visit_hit"];
		$host=$object["visit_host"];
		if(empty($res->t[$t]->hit)) $res->t[$t]->hit=0;
		if(empty($res->t[$t]->host)) $res->t[$t]->host=0;
		$res->t[$t]->hit+=$hit;
		$res->t[$t]->host+=$host;
		$res->total_hit+=$hit;
		$res->total_host+=$host;
	}
	return $res;
}

function getall_statistics3($type,$z,$sql_time,$period,$save_time=0){
	global $db, $select_year, $select_month, $select_day;

	if(!empty($z)) $sql_zone=" AND visit_zone=$z";
	else $sql_zone='';
	
	$sql="SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=$period".$sql_zone.$sql_time;
	$tstat=getall_statistics($sql,Array(),0,$save_time);
	//hours
	if($period==0 && (date_to_xday()!=$select_day || $type==2) && empty($tstat)){
		//day on hours
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=1".$sql_zone." AND visit_time=".date_to_xday(),$tstat,date_to_xday(),$save_time);
	}
	//days
	if($period==1 && date_to_xmonth()==$select_month){
		//last hours collect
		if($type!=2) $tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=0".$sql_zone." AND visit_time>=".((date_to_xday()-1)*24),$tstat,date_to_xday(),$save_time);
	}
	//month
	if($period==2 && date_to_xyear()==$select_year){
		//last hours collect
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=0".$sql_zone." AND visit_time>=".((date_to_xday()-1)*24),$tstat,date_to_xmonth(),$save_time);
		//last days collect
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=1".$sql_zone." AND visit_time>=".(date_to_xday(0,0,1)),$tstat,date_to_xmonth(),$save_time);
	}
	//year		
	if($period==3){
		//last hours collect
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=0".$sql_zone." AND visit_time>=".((date_to_xday()-1)*24),$tstat,date_to_xyear(),$save_time);
		//last days collect
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=1".$sql_zone." AND visit_time>=".(date_to_xday(0,0,1)),$tstat,date_to_xyear(),$save_time);
		//month collect
		$tstat=getall_statistics("SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=2".$sql_zone." AND visit_time>=".((date_to_xyear()-1)*12),$tstat,date_to_xyear(),$save_time);
	}
	return $tstat;
}

function echo_table($days14,$day_start,$day_end,$period=0,$hide_hosts=0){	
	if(empty($day_start) || empty($day_end)){
		$day_start=0;
		$day_end=0;
		foreach($days14->t AS $day=>$stat){
			if($day_start==0 || $day_start>$day) $day_start=$day;
			if($day_end==0 || $day_end<$day) $day_end=$day;
		}
	}

	// высчитываем максимальный показатель хитов за это время
	$max_day14_hit=0;
	$max_day14_host=0;
	foreach($days14->t AS $day=>$stat){
		if($max_day14_hit<=$stat->hit) $max_day14_hit=$stat->hit;
		if(!$hide_hosts && $max_day14_host<=$stat->host) $max_day14_host=$stat->host;
	}
	$host_multyper=1;
	if($max_day14_host<$max_day14_hit/3 && $max_day14_host!=0){
		$host_multyper=($max_day14_hit/3)/$max_day14_host;
	}
	$day14_percent=$max_day14_hit/100;
	if($day14_percent==0) $day14_percent=1;
	$height_constant=120;
	if($period==0 && $day_end-$day_start==13) $period=1;
	
	echo '<table class="records-fixed" cellpadding="3" cellspacing="1"><tr>';
	// строим таблицу - график
	echo '<td></td>';
	for($i=$day_start;$i<=$day_end;$i++){
		if(isset($stat)) unset($stat);
		if(!empty($days14->t[$i])){
			if(!$hide_hosts) $stat->host=$days14->t[$i]->host;
			$stat->hit=$days14->t[$i]->hit;
		} else {
			$stat->hit=0;
			$stat->host=0;
		}
		echo '<td valign="bottom">';
		$hit_height=floor($height_constant*(($stat->hit/$day14_percent)/100));
		if(!$hide_hosts){
			$host_height=floor($height_constant*(($stat->host/$day14_percent)/100));
			$x=$host_height*$host_multyper;
			$host_height=$x;
			$margin=$hit_height-$host_height;		
			echo '<div style="width: 40px; height: '.($hit_height-$margin).'px; background-color: #F8E1E1; padding-top: '.($margin).'px;"><div style="margin-left: 3px; width: 34px; background-color: #E3F8E1; padding-top: 0px; font-size: 12px; height: '.$host_height.'px;" align="center"></div></div>';
		} else echo '<div style="width: 40px; height: '.$hit_height.'px; background-color: #F8E1E1; padding-top: 0px;"></div>';
		echo '</td>';
	}
	echo '</tr><tr>';
	// строим таблицу - дата
	if($period==0) echo '<td>'.lng('Hours').':</td>';
	if($period==1) echo '<td>'.lng('Days').':</td>';
	if($period==2) echo '<td>'.lng('Month').':</td>';
	if($period==3) echo '<td>'.lng('Years').':</td>';
	for($i=$day_start;$i<=$day_end;$i++){
		if($period==0){
			echo '<td align="center" class="td-date">'.(xhour_to_hour($i)+1).'</td>';
		}
		if($period==1){
			$date=xday_to_date(/*$day*/$i);
			$d=explode('-',$date);
			$w=date_to_new_format($date,'w');
			$dicon='';
			if($w==0 || $w==6) $dicon=2;
			echo '<td align="center" class="td-date">'.se('date'.$dicon,'','','width="10"',0,3,0).$d[2].'</td>';
		}
		if($period==2){
			$date=explode('-',xmonth_to_date($i));
			echo '<td align="center" class="td-date">'.get_spec_month2($date[1]).'</td>';
		}
		if($period==3){
			$date=explode('-',xyear_to_date($i));
			echo '<td align="center" class="td-date">'.$date[0].'</td>';
		}
	}
	echo '</tr><tr>';
	
	$current_hour=date_to_xhour();
	$current_day=date_to_xday();
	$current_month=date_to_xmonth();
	$current_year=date_to_xyear();	
	
	// строим таблицу - визиты
	echo '<td>'.lng('Visits').':</td>';
	for($i=$day_start;$i<=$day_end;$i++){
		if(isset($stat)) unset($stat);
		if(!empty($days14->t[$i])) $stat=$days14->t[$i];
		else {
			$stat->hit=0;
			$stat->host=0;
		}
		$is_future=0;
		if($period==0 && $i>$current_hour) $is_future=1;
		if($period==1 && $i>$current_day) $is_future=1;
		if($period==2 && $i>$current_month) $is_future=1;
		if($period==3 && $i>$current_year) $is_future=1;
		if(!$is_future) echo '<td align="center" class="td-num">'.$stat->hit.'</td>';
		else echo '<td class="td-empty">&nbsp;</td>';
	}
	echo '</tr>';
	
	if(!$hide_hosts){
		echo '<tr>';
		// строим таблицу - хосты
		echo '<td>'.lng('Hosts').':</td>';
		for($i=$day_start;$i<=$day_end;$i++){
			if(isset($stat)) unset($stat);
			if(!empty($days14->t[$i])) $stat=$days14->t[$i];
			else {
				$stat->hit=0;
				$stat->host=0;
			}
			$is_future=0;
			if($period==0 && $i>$current_hour) $is_future=1;
			if($period==1 && $i>$current_day) $is_future=1;
			if($period==2 && $i>$current_month) $is_future=1;
			if($period==3 && $i>$current_year) $is_future=1;
			if(!$is_future) echo '<td align="center" class="td-num">'.$stat->host.'</td>';
			else echo '<td class="td-empty">&nbsp;</td>';
		}
		echo '</tr>';
	}
	echo '</table>';
}

function show_sources($source_domain=0){
	global $db,$select_day,$select_month,$select_year,$z,$period;
	if(!empty($source_domain)) $url_pref=getrowval("SELECT * FROM visit_data WHERE data_id=".$source_domain,"data_name");
	else $url_pref='';
	$sql_t='';
	if($period==0){
		$sql_t=' AND source_day='.$select_day;
	}
	if($period==1){
		$tmp=explode('-',xmonth_to_date($select_month));
		$tmp2=explode('-',xmonth_to_date($select_month+1));
		$date1=date_to_xday($tmp[0],$tmp[1],1);
		$date2=date_to_xday($tmp2[0],$tmp2[1],1)-1;
		$sql_t=' AND source_day>='.$date1.' AND source_day<='.$date2;
	}
	if($period==2){
		$tmp=explode('-',xyear_to_date($select_year));
		$tmp2=explode('-',xyear_to_date($select_year+1));
		$date1=date_to_xday($tmp[0],1,1);
		$date2=date_to_xday($tmp2[0],1,1)-1;
		$sql_t=' AND source_day>='.$date1.' AND source_day<='.$date2;
	}
	if(empty($source_domain)) $dmns=getall($db,"SELECT * FROM visit_source WHERE source_zone=$z AND source_type=0".$sql_t);
	else $dmns=getall($db,"SELECT * FROM visit_source WHERE source_zone=$z AND source_type=1 AND source_domain=$source_domain".$sql_t);
	if(!empty($source_domain)){
		$tmp=getall($db,"SELECT * FROM visit_source WHERE source_zone=$z AND source_type=0 AND source_domain=".$source_domain.$sql_t);
		$total_visit=0;
		foreach($tmp AS $tmp2) $total_visit+=$tmp2["source_visit"];
	} else $total_visit=0;
	$dmc=Array();
	$dm_data=Array();
	if(!empty($dmns)) foreach($dmns AS $dmn){
		if(empty($source_domain)) $var=$dmn["source_domain"];
		else $var=$dmn["source_data"];
		if(empty($dmc[$var])) $dmc[$var]=$dmn["source_visit"];
		else $dmc[$var]+=$dmn["source_visit"];
		$dm_data[$var]=$var;
	}
	if(!empty($dmc)){
		$data=getall6($db,"SELECT data_id, data_name FROM visit_data WHERE data_id IN (".implode(',',$dm_data).")","data_id","data_name");
		if(!empty($source_domain)){
			$total_visit2=0;
			foreach($dmc AS $dom=>$visit) $total_visit2+=$visit;
			if($total_visit>$total_visit2){			
				$margin=$total_visit-$total_visit2;
				$dmc[-1]=$margin;
			}
		}
		arsort($dmc);
		if(empty($source_domain)) echo '<h2>'.lng('Sources').'</h2>';
		else echo '<h2>'.lng('Source pages').'</h2>';
		$num=-1;
		$pages=0;
		foreach($dmc AS $dom=>$visit){
			if($num==-1 || $num==19){
				$pages++;
				if($num!=-1) echo '</table></div>';
				echo '<div id="spage'.$pages.'"'.($num!=-1?' style="display: none;"':'').'><table id="records" cellpadding="3" cellspacing="1">';
				echo '<tr>';
				if(empty($source_domain)) echo '<th>'.lng('Domain').'</th>';
				else echo '<th>URL</th>';
				echo '<th width="80">'.lng('Visits').'</th>';
				if(empty($source_domain)) echo '<th width="80">'.lng('Actions').'</th>';
				echo '</tr>';
				$num=0;
			} else $num++;	
			echo '<tr>';
			if($dom==-1) echo '<td>'.lng('From the main page').'</td>';
			else if(!empty($data[$dom])) echo '<td><a href="http://'.$url_pref.$data[$dom].'" target="_blank">'.$url_pref.$data[$dom].'</a></td>';
			else echo '<td>'.lng('Source unknown').'</td>';
			echo '<td>'.$visit.'</td>';
			if(empty($source_domain)) echo '<td><a href="statistics?z='.$z.'&period='.($period==0?-1:$period).'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'&source='.$dom.'">'.lng('More').'</a></td>';
			echo '</tr>';
		}
		if($num!=-1) echo '</table></div>';	
		if($pages>1){
			echo '<div style="height: 40px;">';
			for($i=1;$i<=$pages;$i++){
				echo '<div id="spager'.$i.'" OnMouseOver="pager_over('.$i.',\'s\');" OnClick="pager_click('.$i.',\'s\');" OnMouseOut="pager_out('.$i.',\'s\',0);" align="center" style="float: left; cursor: pointer; border: 1px solid #1076DC; '.($i!=1?'background-color: #E6EFF6;color: #000000;':'background-color: #1076DC;color: #FFFFFF;').' padding: 5px; margin: 2px;">'.$i.'</div>';
			}
			echo '</div>';
		}
	}
}

function show_triggers($sql_time,$check_month=0){
	global $period,$z,$db,$select_year,$select_month,$select_day;
	
	$tstat=getall_statistics3(3,$z,$sql_time,$period,$check_month);
		
	$tt=Array();
	$trig=Array();
	if(!empty($tstat)) foreach($tstat AS $trigger=>$stat){
		$tt[$stat->total_host][]=$trigger;
	}
	krsort($tt);
	if(!empty($tt)) foreach($tt AS $ttime) foreach($ttime AS $trigger){
		$trig[$trigger]=$trigger;
	}
	if(!empty($tt)){
		echo '<h2>'.lng('Events').'</h2>';
		$data=implode(',',$trig);
		$data=getall6($db,"SELECT data_id, data_name FROM visit_data WHERE data_id IN (".$data.")","data_id","data_name");
		$num=-1;
		$pages=0;
		if(!empty($tt)) foreach($tt AS $ttime) foreach($ttime AS $tid){
			if($num==-1 || $num==19){
				$pages++;
				if($num!=-1) echo '</table></div>';
				echo '<div id="tpage'.$pages.'"'.($num!=-1?' style="display: none;"':'').'><table id="records" cellpadding="3" cellspacing="1">';
				echo '<tr>';
				echo '<th>Событие</th>';
				echo '<th width="80">'.lng('Hosts').'</th>';
				echo '<th width="80">'.lng('Visits').'</th>';
				echo '<th width="80">'.lng('Actions').'</th>';
				echo '</tr>';
				$num=0;
			} else $num++;
		
			$host=$tstat[$tid]->total_host;
			$hit=$tstat[$tid]->total_hit;
			echo '<tr>';
			if(!empty($data[$tid])) echo '<td>'.$data[$tid].'</td>';
			else echo '<td>'.lng('Name unknown').'</td>';
			echo '<td>'.$host.'</td><td>'.$hit.'</td>';
			$x_select_month=$select_month;
			if($check_month && !empty($tstat[$tid]->last_t) && $period==1){
				$x_select_month=date_to_xmonth(xday_to_date($tstat[$tid]->last_t));
			}
			echo '<td><a href="statistics?'.(!empty($z)?'z='.$z.'&':'').'select_type=3&select_object='.$tid.'&period='.($period==0?'-1':$period).'&select_day='.$select_day.'&select_month='.$x_select_month.'&select_year='.$select_year.'">'.lng('More').'</a></td>';
			echo '</tr>';
		}
		if($num!=-1) echo '</table></div>';	
		if($pages>1){
			echo '<div style="height: 40px;">';
			for($i=1;$i<=$pages;$i++){
				echo '<div id="tpager'.$i.'" OnMouseOver="pager_over('.$i.',\'t\');" OnClick="pager_click('.$i.',\'t\');" OnMouseOut="pager_out('.$i.',\'t\',0);" align="center" style="float: left; cursor: pointer; border: 1px solid #1076DC; '.($i!=1?'background-color: #E6EFF6;color: #000000;':'background-color: #1076DC;color: #FFFFFF;').' padding: 5px; margin: 2px;">'.$i.'</div>';
			}
			echo '</div>';
		}
	}
}

$section='';
$z='';
$action='';
if(!empty($_GET["section"])) $section=$_GET["section"];
if(!empty($_POST["section"])) $section=$_POST["section"];
if(!empty($_GET["z"])) $z=$_GET["z"];
if(!empty($_POST["z"])) $z=$_POST["z"];
if(!empty($_GET["action"])) $action=$_GET["action"];
if(!empty($_POST["action"])) $action=$_POST["action"];

if(!empty($_GET["period"])) $period=$_GET["period"];
if(!empty($_POST["period"])) $period=$_POST["period"];
if(!empty($_GET["select_month"])) $select_month=$_GET["select_month"];
if(!empty($_POST["select_month"])) $select_month=$_POST["select_month"];
if(!empty($_GET["select_month2"])) $select_month2=$_GET["select_month2"];
if(!empty($_POST["select_month2"])) $select_month2=$_POST["select_month2"];
if(!empty($_GET["select_year"])) $select_year=$_GET["select_year"];
if(!empty($_POST["select_year"])) $select_year=$_POST["select_year"];
if(!empty($_GET["select_day"])) $select_day=$_GET["select_day"];
if(!empty($_POST["select_day"])) $select_day=$_POST["select_day"];

if(!empty($_GET["select_type"])) $select_type=$_GET["select_type"];
if(!empty($_POST["select_type"])) $select_type=$_POST["select_type"];
if(!empty($_GET["select_object"])) $select_object=$_GET["select_object"];
if(!empty($_POST["select_object"])) $select_object=$_POST["select_object"];
if(!empty($_GET["source"])) $source=$_GET["source"];
if(!empty($_POST["source"])) $source=$_POST["source"];

if(empty($source)) $source=0;
if(empty($select_type)) $select_type=0;
if(empty($select_object)) $select_object=0;
if(empty($period)) $period=0;
if(empty($select_month)) $select_month=0;
if(empty($select_year)) $select_year=0;
if(empty($select_day)) $select_day=0;
//if(!empty($select_month2)) $select_month=$select_month2;//при включении этого параметра слетает селектор месяцев

if(!empty($z) && !check_zone($z,'rules')){
	unset($z);
}

if(empty($section) && $user->super){
	echo '<div align="right">'.se('back_config','statistics?section=settings').'</div>';
}

if(!empty($z)){
	getrow($db,"SELECT zone_id,zone_name,zone_domain,zone_folder FROM main_zone WHERE zone_id=$z");
	echo '<h2>'.$db->Record["zone_name"].' '.$db->Record["zone_domain"].' '.$db->Record["zone_folder"].'</h2>';
	echo '<div><a href="statistics?period='.$period.'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'">'.si('back').lng('Back to main statistics').' </a></div>';
	if(!empty($select_object)){
		if($select_type==3) echo '<h2>'.lng('Event statistics').' "'.getrowval("SELECT * FROM visit_data WHERE data_id=$select_object","data_name").'"</h2>';
		if($select_type==2){
			$ourl=get_row_url($select_object);
			if(empty($ourl)) echo '<h2>'.lng('Object statistics').' "'.get_basename($select_object).'"</h2>';
			else echo '<h2>'.lng('Object statistics').' "<a href="'.$ourl.'" target="_blank">'.get_basename($select_object).'</a>"<a href="'.$ourl.'" target="_blank">'.si('anchor').'</a></h2>';
		}
		getrow($db,"SELECT zone_id,zone_name,zone_domain,zone_folder FROM main_zone WHERE zone_id=$z");
		echo '<div><a href="statistics?period='.$period.'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'&z='.$z.'">'.si('back').' '.lng('Back to domain statistics').' '.$db->Record["zone_name"].' '.$db->Record["zone_domain"].' '.$db->Record["zone_folder"].'</a></div>';
	}
	if(!empty($source)){
		echo '<h2>'.lng('Statistics of visitors from domain').' "'.getrowval("SELECT * FROM visit_data WHERE data_id=$source","data_name").'"</h2>';
		getrow($db,"SELECT zone_id,zone_name,zone_domain,zone_folder FROM main_zone WHERE zone_id=$z");
		echo '<div><a href="statistics?period='.$period.'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'&z='.$z.'">'.si('back').' '.lng('Back to domain statistics').' '.$db->Record["zone_name"].' '.$db->Record["zone_domain"].' '.$db->Record["zone_folder"].'</a></div>';
	}
} else if(!empty($select_type)){
	if(!empty($select_object)){
		if($select_type==3) echo '<h2>'.lng('Event statistics').' "'.getrowval("SELECT * FROM visit_data WHERE data_id=$select_object","data_name").'"</h2>';
		if($select_type==2){
			$ourl=get_row_url($select_object);
			if(empty($ourl)) echo '<h2>'.lng('Object statistics').' "'.get_basename($select_object).'"</h2>';
			else echo '<h2>'.lng('Object statistics').' "<a href="'.$ourl.'" target="_blank">'.get_basename($select_object).'</a>"<a href="'.$ourl.'" target="_blank">'.si('anchor').'</a></h2>';
		}
		echo '<div><a href="statistics?period='.$period.'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'">'.si('back').' '.lng('Back to main statistics').'</a></div>';
	}
}

// блок выбора периода
if(empty($section)){
	$xmonth=date_to_xmonth();
	$xday=date_to_xday();
	//дни
	$first_month=getrowval("SELECT * FROM visit_object WHERE visit_period=1 ORDER BY visit_time LIMIT 1","visit_time");
	$last_month=getrowval("SELECT * FROM visit_object WHERE visit_period=1 ORDER BY visit_time DESC LIMIT 1","visit_time");	
	if($last_month!=$xday) $last_month=$xday;
	//месяца
	$first_year=getrowval("SELECT * FROM visit_object WHERE visit_period=2 ORDER BY visit_time LIMIT 1","visit_time");
	$last_year=getrowval("SELECT * FROM visit_object WHERE visit_period=2 ORDER BY visit_time DESC LIMIT 1","visit_time");		
	if($last_year!=$xmonth) $last_year=$xmonth;

	echo '<form action="statistics" method="get">';
	if(!empty($z)) echo '<input type="hidden" name="z" value="'.$z.'">';
	if(!empty($select_type)) echo '<input type="hidden" name="select_type" value="'.$select_type.'">';
	if(!empty($select_object)) echo '<input type="hidden" name="select_object" value="'.$select_object.'">';
	if(!empty($source)) echo '<input type="hidden" name="source" value="'.$source.'">';
	echo '<div><b>'.lng('Select statistics period').'</b>: ';
	echo '<select name="period" class="button" OnChange="
		var ds=document.getElementById(\'select_day\');
		var ms=document.getElementById(\'select_month\');
		var ys=document.getElementById(\'select_year\');
		ds.style.display=\'none\';
		ms.style.display=\'none\';
		ys.style.display=\'none\';';
	if($select_type==2)  echo ' if(this.selectedIndex==0) ms.style.display=\'\'; if(this.selectedIndex==1) ys.style.display=\'\'; ';
	else if(empty($z) && empty($select_type)) echo ' if(this.selectedIndex==1) ds.style.display=\'\'; if(this.selectedIndex==2) ms.style.display=\'\'; if(this.selectedIndex==3) ys.style.display=\'\'; ';
	else echo ' if(this.selectedIndex==0) ds.style.display=\'\'; if(this.selectedIndex==1) ms.style.display=\'\'; if(this.selectedIndex==2) ys.style.display=\'\'; ';
	echo '">';
	if(empty($z) && empty($select_type)) echo '<option value="0">'.lng('14 days').'</option>';
	if($select_type!=2) echo '<option value="-1"'.($period==-1?' selected':'').'>'.lng('Per day').'</option>';
	echo '<option value="1"'.($period==1?' selected':'').'>'.lng('Per month').'</option>';
	echo '<option value="2"'.($period==2?' selected':'').'>'.lng('Per year').'</option>';
	echo '<option value="3"'.($period==3?' selected':'').'>'.lng('All time').'</option>';
	echo '</select>';
	
	// список дней
	echo '<span id="select_day"'.($period==-1?'':' style="display: none;"').'>';
	echo ' <select name="select_month2" class="button" OnChange="visit_month_select(this);">';
	$f=true;
	$check=Array();
	$prep_month=Array();
	$lmonth=0;
	for($i=$last_month;$i>$first_month;$i--){	
		$date=xday_to_date($i);
		$tmp=explode('-',$date);
		$day=$tmp[2];
		$month=$tmp[1];
		$year=$tmp[0];
		if($lmonth!=$month.$year && empty($check[$month.$year])){
			$check[$month.$year]=1;
			if($month==12 && !$f) echo '<option disabled>'.$year.'</option>';
			$xmonth=date_to_xmonth($year,$month);
			echo '<option value="'.$xmonth.'"'.($select_month2==$xmonth?' selected':'').'>'.get_spec_month2($month).'</option>';
			$prep_month[$xmonth]=$xmonth;
			$f=false;
		}
		$lmonth=$month.$year;
	}
	if(empty($last_month) || empty($check)){
		echo '<option value="'.date_to_xmonth().'">'.get_spec_month2(date('m')).'</option>';
		$prep_month[date_to_xmonth()]=date_to_xmonth();
	}
	echo '</select>';
	echo ' <select name="select_day" id="select_day" class="button">';
	if(empty($select_month)) $select_month2=date_to_xmonth();
	else $select_month2=$select_month;
	$tmp=xmonth_to_date($select_month2);
	$max_days=date_to_new_format($tmp,'t');
	$start_xday=date_to_xday($tmp);
	$select_day2=$select_day;
	if(empty($select_day2)) $select_day2=date_to_xday();
	for($i=1;$i<=$max_days;$i++){
		$xd=$i+$start_xday-1;
		$w=date_to_weekday2(xday_to_date($xd));//week day
		echo '<option value="'.$xd.'"'.($select_day2==$xd?' selected':'').'>'.$i.' ('.$w.')</option>';
	}
	echo '</select>';
	echo '</span>';
	
	// список месяцев
	echo ' <select name="select_month" id="select_month" class="button"'.($period==1 || ($period==0 && $select_type==2)?'':' style="display: none;"').'>';
	$f=true;
	$check=Array();
	$lyear=0;
	for($i=$last_month;$i>$first_month;$i--){
		$date=xday_to_date($i);
		$tmp=explode('-',$date);
		$day=$tmp[2];
		$month=$tmp[1];
		$year=$tmp[0];
		if($lyear!=$month.$year && empty($check[$month.$year])){
			$check[$month.$year]=1;
			if($month==12 && !$f) echo '<option disabled>'.$year.'</option>';
			$xmonth=date_to_xmonth($year,$month);
			echo '<option value="'.$xmonth.'"'.($select_month==$xmonth?' selected':'').'>'.get_spec_month2($month).'</option>';
			$f=false;
		}
	}
	if(empty($last_month) || empty($check)) echo '<option value="'.date_to_xmonth().'">'.get_spec_month2(date('m')).'</option>';
	echo '</select>';
	
	// список лет
	echo ' <select name="select_year" id="select_year" class="button"'.($period==2?'':' style="display: none;"').'>';
	$check=Array();
	for($i=$last_year;$i>$first_year;$i--){
		$date=xmonth_to_date($i);
		$tmp=explode('-',$date);
		$month=$tmp[1];
		$year=$tmp[0];
		if($i==$last_year || ($month==12 && empty($check[$year]))){
			$check[$year]=1;
			$xyear=date_to_xyear($year);
			echo '<option value="'.$xyear.'"'.($select_year==$xyear?' selected':'').'>'.$year.'</option>';
		}
	}
	if(empty($last_year)) echo '<option value="'.date_to_xyear().'">'.date('Y').'</option>';
	echo '</select>';
	echo '<input type="submit" value="'.lng('Show').'" class="button">';
	echo '</form>';
	echo '<br><br>';
}

// подробная информация по источнику
if(!empty($source)){
	if($period==-1) $period=0;	
	// график посещаемости
	if($period>0){
		if($period==1){
			if(!empty($r)) unset($r);
			$tmp=explode('-',xmonth_to_date($select_month));
			$tmp2=explode('-',xmonth_to_date($select_month+1));
			$start=date_to_xday($tmp[0],$tmp[1],1);
			$end=date_to_xday($tmp2[0],$tmp2[1],1)-1;
			$v=getall($db,"SELECT * FROM visit_source WHERE source_type=0 AND source_domain=$source AND source_day>=$start AND source_day<=$end AND source_zone=".$z);
			$r->t=Array();
			if(!empty($v)) foreach($v AS $tv){
				$d=$tv["source_day"];
				if(empty($r->t[$d])) $r->t[$d]->hit=0;
				$r->t[$d]->hit+=$tv["source_visit"];
			}
		}
		if($period==2){
			if(!empty($r)) unset($r);
			$tmp=explode('-',xyear_to_date($select_year));
			$tmp2=explode('-',xyear_to_date($select_year+1));
			$start=date_to_xmonth($tmp[0],1);
			$end=date_to_xmonth($tmp2[0],1)-1;
			$start2=date_to_xday($tmp[0],1);
			$end2=date_to_xday($tmp2[0],1)-1;
			$v=getall($db,"SELECT * FROM visit_source WHERE source_type=0 AND source_domain=$source AND source_day>=$start2 AND source_day<=$end2 AND source_zone=".$z);
			$r->t=Array();
			if(!empty($v)) foreach($v AS $tv){
				$d=$tv["source_day"];
				$tmp=explode('-',xday_to_date($d));
				$m=date_to_xmonth($tmp[0],$tmp[1]);
				if(empty($r->t[$m])) $r->t[$m]->hit=0;
				$r->t[$m]->hit+=$tv["source_visit"];
			}
		}
		if($period==3){
			if(!empty($r)) unset($r);
			$v=getall($db,"SELECT * FROM visit_source WHERE source_type=0 AND source_domain=$source AND source_zone=".$z);
			$r->t=Array();
			if(!empty($v)) foreach($v AS $tv){
				$d=$tv["source_day"];
				$tmp=explode('-',xday_to_date($d));
				$y=date_to_xyear($tmp[0]);
				if(empty($r->t[$y])) $r->t[$y]->hit=0;
				$r->t[$y]->hit+=$tv["source_visit"];
			}
		}
		
		if(empty($start)) $start=0;
		if(empty($end)) $end=0;
		if(!empty($r)) echo_table($r,$start,$end,$period,1);
	}
	
	// вывод страниц источника
	show_sources($source);
}

// вывод статистики по периодам
if(!empty($period) && empty($source)){
	if($period==-1) $period='0';
	$sql="SELECT * FROM visit_object WHERE visit_type=".$select_type." AND visit_period=".$period;
	if(!empty($select_type) && !empty($select_object)) $sql.=' AND visit_object='.$select_object;
	else if(!empty($z)) $sql.=' AND visit_object='.$z;
	else $sql.=$add_sql;
	
	if($period==0){
		$start=($select_day-1)*24;
		$end=$select_day*24-1;
	}
	if($period==1){
		$tmp=explode('-',xmonth_to_date($select_month));
		$tmp2=explode('-',xmonth_to_date($select_month+1));
		$start=date_to_xday($tmp[0],$tmp[1],1);
		$end=date_to_xday($tmp2[0],$tmp2[1],1)-1;
	}
	if($period==2){
		$tmp=explode('-',xyear_to_date($select_year));
		$tmp2=explode('-',xyear_to_date($select_year+1));
		$start=date_to_xmonth($tmp[0],1);
		$end=date_to_xmonth($tmp2[0],1)-1;
	}

	if(!empty($start)) $sql_time=' AND visit_time>='.$start.' AND visit_time<='.$end;
	else $sql_time='';
		
	$visits=getall_statistics2($sql.$sql_time);

	//if(empty($z)) $details=getall_statistics($sql.$sql_time); // <-? WTF ?
	
	if(!empty($select_type) && !empty($select_object)) $sqlx=' AND visit_object='.$select_object;
	else if(!empty($z)) $sqlx=' AND visit_object='.$z;
	else $sqlx=$add_sql;
	
	if($period==1 && date_to_xmonth()==$select_month){
		if($select_type!=2){
			$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=0 AND visit_time>=".((date_to_xday()-1)*24).$sqlx);
			$visits->t[date_to_xday()]->hit=$add_visits->total_hit;
			$visits->t[date_to_xday()]->host=$add_visits->total_host;
		}
	}
	if($period==2 && date_to_xyear()==$select_year){
		$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=1 AND visit_time>".(date_to_xday()-date('d')).$sqlx);
		$visits->t[date_to_xmonth()]->hit=$add_visits->total_hit;
		$visits->t[date_to_xmonth()]->host=$add_visits->total_host;

		$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=0 AND visit_time>=".((date_to_xday()-1)*24).$sqlx);
		$visits->t[date_to_xmonth()]->hit+=$add_visits->total_hit;
		$visits->t[date_to_xmonth()]->host+=$add_visits->total_host;
	}
	if($period==3){
		$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=2 AND visit_time>".(date_to_xmonth()-date('m')).$sqlx);
		$visits->t[date_to_xyear()]->hit=$add_visits->total_hit;
		$visits->t[date_to_xyear()]->host=$add_visits->total_host;
		
		$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=1 AND visit_time>".(date_to_xday()-date('d')).$sqlx);
		$visits->t[date_to_xyear()]->hit+=$add_visits->total_hit;
		$visits->t[date_to_xyear()]->host+=$add_visits->total_host;

		$add_visits=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=$select_type AND visit_period=0 AND visit_time>=".((date_to_xday()-1)*24).$sqlx);
		$visits->t[date_to_xyear()]->hit+=$add_visits->total_hit;
		$visits->t[date_to_xyear()]->host+=$add_visits->total_host;
	}
	
	if(empty($start)) $start=0;
	if(empty($end)) $end=0;
	echo_table($visits,$start,$end,$period);
	
	if(empty($z) && empty($select_type)){
		// Триггеры
		show_triggers($sql_time);
		
		// Статистика по зонам	
		$stat_zone=getall_statistics3(0,0,$sql_time.$add_sql,$period);
		$zt=Array();
		foreach($zone AS $id=>$z)if(empty($z["zone_redirect"]) && check_zone($z["zone_id"],'rules')){
			if(!empty($stat_zone[$z["zone_id"]])) $time=$stat_zone[$z["zone_id"]]->total_host;
			else $time=0;
			$zt[$time][]=$id;
		}
		krsort($zt);
		
		echo '<h2>'.lng('Domains').'</h2>';

		echo '<table id="records" cellpadding="3" cellspacing="1">';
		echo '<tr>';
		echo '<th>'.lng('Domain').'</th>';
		echo '<th>'.lng('Hosts').'</th>';
		echo '<th>'.lng('Visits').'</th>';
		echo '<th>'.lng('Actions').'</th>';
		echo '</tr>';
		if(!empty($zt)) foreach($zt AS $ztime) foreach($ztime AS $z_num){
			$z=$zone[$z_num];
			$z_id=$z["zone_id"];
			echo '<tr>';
			
			$dom=$z["zone_domain"];
			if(!empty($dom)) $dom.=' ';
			$dom.=$z["zone_folder"];
			if(!empty($dom)) $dom=' / '.$dom;
			echo '<td>'.$z["zone_name"].$dom.'</td>';
			
			if(empty($stat_zone[$z_id])){
				$stat_zone[$z_id]->total_host=0;
				$stat_zone[$z_id]->total_hit=0;
			}	
			
			echo '<td>'.$stat_zone[$z_id]->total_host.'</td>';	
			echo '<td>'.$stat_zone[$z_id]->total_hit.'</td>';
			$period2=$period;
			if($period2==0) $period2=-1;
			echo '<td><a href="statistics?z='.$z_id.'&period='.$period2.'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'">'.lng('More').'</a></td>';
			
			echo '</tr>';
		}
		echo '</table>';
		
	} else if(empty($select_type)) {
		echo '<div style="margin-top: 10px;">'.lng('Online').': '.get_online($z).'</div>';	
	
		// источники
		show_sources();
				
		// триггеры
		show_triggers($sql_time);

		// поиск посещаемости объектов
		
		//$sql="SELECT * FROM visit_object WHERE visit_period=$period AND visit_type=2 AND visit_zone=$z".$sql_time;
		//$ostat=getall_statistics($sql);
		$ostat=getall_statistics3(2,$z,$sql_time,$period);
		$ot=Array();
		if(!empty($ostat)) foreach($ostat AS $obj=>$stat){
			$ot[$stat->total_host][]=$obj;
		}
		krsort($ot);
		
		if(!empty($ot)){
			echo '<h2>'.lng('Objects').'</h2>';
			$oname=Array();
			$oids=Array();
			$count=0;
			$exceed_limit=false;
			if(!empty($ot)) foreach($ot AS $hosts=>$otime){
				foreach($otime AS $oid){
					$oids[$oid]=$oid;
					$count++;
					if($count>=600){
						$exceed_limit=true;
						break;
					}
				}
				if($count>=600) break;
			}
			$rws=getall($db,"SELECT row_id,row_table FROM main_row WHERE row_id IN (".implode(',',$oids).")");
			$otbls=Array();
			$otbls2=Array();
			foreach($rws AS $rw){
				$otbls[$rw["row_table"]][$rw["row_id"]]=$rw["row_id"];	
				$otbls2[$rw["row_table"]]=$rw["row_table"];
			}
			$mcols=getall4($db,"SELECT table_id,major_col FROM main_table WHERE table_id IN (".implode(',',$otbls2).")","table_id");
			foreach($otbls AS $tbl_id=>$rows){
				if(!empty($mcols[$tbl_id]["major_col"])) $oname=getall4($db,"SELECT value_col,value_row,value_value FROM row_value WHERE value_col=".$mcols[$tbl_id]["major_col"]." AND value_row IN (".implode(',',$rows).")","value_row",$oname);
			}
			
			$num=-1;
			$pages=0;
			$count=0;
			if(!empty($ot)) foreach($ot AS $otime) foreach($otime AS $oid){
				if($num==-1 || $num==19){
					$pages++;
					if($num!=-1) echo '</table></div>';
					echo '<div id="opage'.$pages.'"'.($num!=-1?' style="display: none;"':'').'><table id="records" cellpadding="3" cellspacing="1">';
					echo '<tr>';
					echo '<th>'.lng('Object').'</th>';
					echo '<th width="80">'.lng('Hosts').'</th>';
					echo '<th width="80">'.lng('Visits').'</th>';
					echo '<th width="80">'.lng('Actions').'</th>';
					echo '</tr>';					
					$num=0;
				} else $num++;				
			
				$host=$ostat[$oid]->total_host;
				$hit=$ostat[$oid]->total_hit;
				echo '<tr>';
				if(!empty($oname[$oid])) echo '<td>'.$oname[$oid]["value_value"].'</td>';
				else echo '<td>'.lng('Name unknown').'</td>';
				echo '<td>'.$host.'</td><td>'.$hit.'</td>';
				echo '<td><a href="statistics?z='.$z.'&select_type=2&select_object='.$oid.'&period='.($period==0?'1':$period).'&select_day='.$select_day.'&select_month='.$select_month.'&select_year='.$select_year.'">'.lng('More').'</a></td>';
				echo '</tr>';
				$count++;
				if($count>=600) break;
			}
			if($num!=-1) echo '</table></div>';
			if($exceed_limit) $pages=30;
			if($pages>1){
				echo '<div style="height: 40px;">';
				for($i=1;$i<=$pages;$i++){
					echo '<div id="opager'.$i.'" OnMouseOver="pager_over('.$i.',\'o\');" OnClick="pager_click('.$i.',\'o\');" OnMouseOut="pager_out('.$i.',\'o\',0);" align="center" style="float: left; cursor: pointer; border: 1px solid #1076DC; '.($i!=1?'background-color: #E6EFF6;color: #000000;':'background-color: #1076DC;color: #FFFFFF;').' padding: 5px; margin: 2px;">'.$i.'</div>';
				}
				echo '</div>';
			}
			if($exceed_limit) echo '<div>'.lng('Showing the first 600 results').'</div>';
		}
	}
}

// главный экран
if(empty($section) && empty($z) && empty($period) && empty($select_type)){	
	if($period==-1) $period=0;
	
	$day_total=date_to_xday();
	$today_hours=($day_total-1)*24;
	$today_hours2=($day_total-2)*24+date('H')/*-1 ?? */;
	$yesterday_hours=($day_total-2)*24;
	$hour24=date_to_xhour()-24;

	$day31=$day_total-31;
	$day_last_month=date_to_xday(0,0,1);

	// Подготовка таблицы зон
	$today=getall_statistics("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=0 AND visit_time>=$today_hours".$add_sql." ORDER BY visit_time");
	$yesterday=getall_statistics("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=0 AND visit_time>=$yesterday_hours AND visit_time<$today_hours2".$add_sql." ORDER BY visit_time");
	$day24=getall_statistics("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=0 AND visit_time>$hour24".$add_sql." ORDER BY visit_time");
	$month31=getall_statistics("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=1 AND visit_time>$day31".$add_sql." ORDER BY visit_time");
	$cur_month=getall_statistics("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=1 AND visit_time>$day_last_month".$add_sql." ORDER BY visit_time");
	
	// Начало таблицы двух блоков
	echo '<br><table cellpadding="0" cellspacing="0"><tr><td align="left" valign="top">';
	
	// Подготовка графика по дням
	$day_start=$day_total-14+1;
	$day_end=$day_total;
	$days14=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=1 AND visit_time>=$day_start AND visit_time<=$day_end".$add_sql." ORDER BY visit_time");
	
	// добавляем сегодняшний день в график
	$today_total_host=0;
	$today_total_hit=0;
	foreach($today AS $id=>$time){
		$today_total_host+=$time->total_host;
		$today_total_hit+=$time->total_hit;
	}
	$days14->t[$day_total]->host=$today_total_host;
	$days14->t[$day_total]->hit=$today_total_hit;

	echo_table($days14,$day_start,$day_end);
	
	// Разделение таблицы двух блоков статистики
	echo '</td><td width="50"></td><td align="center" valign="top"><div style="padding-bottom: 10px;"><b>'.lng('Statistics for the last 24 hours').'</b></div>';
	
	// Подготавливаем статистику по часам
	$hours24=getall_statistics2("SELECT * FROM visit_object WHERE visit_type=0 AND visit_period=0 AND visit_time>=$hour24".$add_sql." ORDER BY visit_time");
	
	$xhour=date_to_xhour();
	$max_hour_hit=0;
	$max_hour_host=0;
	for($hr=24;$hr>0;$hr--){
		$xhr=$xhour-$hr;
		if(empty($hours24->t[$xhr])){
			$hours24->t[$xhr]->hit=0;
			$hours24->t[$xhr]->host=0;
		}
		if($max_hour_hit<=$hours24->t[$xhr]->hit) $max_hour_hit=$hours24->t[$xhr]->hit;
		if($max_hour_host<=$hours24->t[$xhr]->host) $max_hour_host=$hours24->t[$xhr]->host;
	}
	$host_multyper=1;
	if($max_hour_host<$max_hour_hit/3 && $max_hour_host!=0){
		$host_multyper=($max_hour_hit/3)/$max_hour_host;
	}
	$hour_percent=$max_hour_hit/100;
	if($hour_percent==0) $hour_percent=1;
	$height_constant=120;
	
	echo '<table class="records-fixed" cellpadding="0" cellspacing="0"><tr>';
	// строим график статистики по часам
	echo '<td valign="top" style="padding-right: 5px;"><div style="float: left; width: 10px; height: 10px; margin-top: 2px; margin-left: 2px; margin-right: 4px;  background-color: #F8E1E1;"></div>'.$max_hour_hit.'<br><br><br><br><div style="float: left; width: 10px; height: 10px; margin-top: 2px; margin-left: 2px; margin-right: 4px; background-color: #F8E1E1;"></div>'.floor($max_hour_hit/2).'</td>';
	for($hr=24;$hr>0;$hr--){
		$xhr=$xhour-$hr;
		$stat=$hours24->t[$xhr];
		echo '<td valign="bottom">';
		$hit_height=floor($height_constant*(($stat->hit/$hour_percent)/100));
		$host_height=floor($height_constant*((($stat->host)/$hour_percent)/100));
		$x=$host_height*$host_multyper;
		$host_height=$x;
		$margin=$hit_height-$host_height;		
		echo '<div style="width: 20px; height: '.($hit_height-$margin).'px; background-color: #F8E1E1; padding-top: '.($margin).'px;"><div style="width: 20px; background-color: #E3F8E1; height: '.$host_height.'px;"></div></div>';
		echo '</td>';
	}
	echo '<td valign="top" style="padding-left: 5px;"><div style="float: right; width: 10px; height: 10px; margin-top: 2px; margin-left: 4px; background-color: #E3F8E1;"></div>'.floor($max_hour_host*$host_multyper).'<br><br><br><br><div style="float: right; width: 10px; height: 10px; margin-top: 2px; margin-left: 4px; background-color: #E3F8E1;"></div>'.floor($max_hour_host/2*$host_multyper).'</td>';
	echo '</tr><tr><td style="padding-top: 3px;">'.lng('Hours').':</td>';
	for($hr=24;$hr>0;$hr--){
		$xhr=$xhour-$hr;
		echo '<td align="center" style="padding-top: 3px; border-top: 2px solid #BBBBBB;">'.(xhour_to_hour($xhr)+1).'</td>';
	}
	echo '<td></td>';
	echo '</tr></table>';
		
	//завершение верхней таблицы с колонками - статистикой по дням и статистикой по часам
	echo '</td></tr></table>';
	
	//echo '<div style="margin-top: 10px;">Онлайн: '.get_online($z).'</div>';	
	echo '<div style="margin-top: 10px;">Онлайн: '.get_online(0).' (5 мин) / '.get_online(0,1).' (1 мин)</div>';	
	
	// Начинаем выводить домены	
	// сортируем по зонам
	$zt=Array();
	foreach($zone AS $id=>$zm)if(empty($zm["zone_redirect"]) && check_zone($zm["zone_id"],'rules')){
		if(!empty($today[$zm["zone_id"]])) $time=$today[$zm["zone_id"]]->total_host;
		else $time=0;
		$zt[$time][]=$id;
	}
	krsort($zt);
	
	// события (триггеры)	
	$sql_time=' AND visit_time>='.(date_to_xday()-14+1);
	$old_period=$period;
	$period=1;
	$select_day=date_to_xday();
	show_triggers($sql_time,1);
	$period=$old_period;
	
	// перечень доменов и его посещаемость
	echo '<h2>'.lng('Domains').'</h2>';

	$vars['th']=Array(
		lng('Domain'),
		lng('Today').' <span style="font-size: 10px;">'.lng('host/visit').'</span>',
		lng('For 24 hours'),
		lng('Current month'),
		lng('For 31 days'),
		lng('Actions')
	);
	sdefine('td_domain',0);
	sdefine('td_today',1);
	sdefine('td_24',2);
	sdefine('td_month',3);
	sdefine('td_31',4);
	sdefine('td_actions',5);
	$vars['rows']=Array();
	if(!empty($zt)) foreach($zt AS $ztime) foreach($ztime AS $z_num){
		$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];
		$z=$zone[$z_num];
		$z_id=$z["zone_id"];
		
		$dom=$z["zone_domain"];
		if(!empty($dom)) $dom.=' ';
		$dom.=$z["zone_folder"];
		if(!empty($dom)) $dom=' / '.$dom;
		$rc[td_domain]=$z["zone_name"].$dom;
		
		if(empty($today[$z_id])){
			$today[$z_id]->total_host=0;
			$today[$z_id]->total_hit=0;
		}
		$p1=''; $p2='';
		if(!empty($yesterday[$z_id])){
			$hom=$today[$z_id]->total_host-$yesterday[$z_id]->total_host;
			$him=$today[$z_id]->total_hit-$yesterday[$z_id]->total_hit;
			if($hom<0) $p1='<span class="red small"> '.$hom.'</span>';
			if($hom>0) $p1='<span class="green small"> +'.$hom.'</span>';
			if($him<0) $p2='<span class="red small"> '.$him.'</span>';
			if($him>0) $p2='<span class="green small"> +'.$him.'</span>';
		}
		$rc[td_today]='<span class="big">'.$today[$z_id]->total_host.'</span>'.$p1.' / <span class="big">'.$today[$z_id]->total_hit.'</span>'.$p2;
		
		if(empty($day24[$z_id])){
			$day24[$z_id]->total_host=0;
			$day24[$z_id]->total_hit=0;
		}
		$rc[td_24]=$day24[$z_id]->total_host.' / '.$day24[$z_id]->total_hit;
		
		if(empty($cur_month[$z_id])){
			$cur_month[$z_id]->total_host=0;
			$cur_month[$z_id]->total_hit=0;
		}
		$rc[td_month]=($cur_month[$z_id]->total_host+$today[$z_id]->total_host).' / '.($cur_month[$z_id]->total_hit+$today[$z_id]->total_hit);
		
		if(empty($month31[$z_id])){
			$month31[$z_id]->total_host=0;
			$month31[$z_id]->total_hit=0;
		}
		$rc[td_31]=($month31[$z_id]->total_host+$today[$z_id]->total_host).' / '.($month31[$z_id]->total_hit+$today[$z_id]->total_hit);
		
		$period2=$period;
		if($period2==0) $period2=-1;
		$rc[td_actions]='<a href="statistics?z='.$z_id.'&period=1&select_month='.date_to_xmonth().'&select_year='.date_to_xyear().'&select_day='.date_to_xday().'">Подробнее</a>';
		
	}
	echo shell_tpl_admin('block/table',$vars);
	
	// Активность пользователей по IP
	echo '<h2>'.lng('IP Activity').'</h2>';
	
	$ips=getall($db,"SELECT SUM(ip_visit) AS `summ`, ip_value FROM visit_ip WHERE ip_type=0 AND ip_time IN (".$xhour.",".($xhour-1).") GROUP BY ip_value ORDER BY summ DESC LIMIT 0, 10");
	
	$vars['th']=Array(
		lng('IP address'),
		lng('Number of visits for the last hour'),
		lng('For 12 hours'),
		lng('For 20 minutes'),
		lng('For 5 minutes')
	);
	sdefine('td_ip',0);
	sdefine('td_hour',1);
	sdefine('td_12',2);
	sdefine('td_20',3);
	sdefine('td_5',4);
	$vars['rows']=Array();
	
	$n_min20=date_to_xhour()*60+date('i')-20;
	$n_min5=date_to_xhour()*60+date('i')-5;
	if(!empty($ips)) foreach($ips AS $ip){
		$r=&$vars['rows'][count($vars['rows'])]; $rc=&$r['cols'];
		$rc[td_ip]=long2ip($ip["ip_value"]);
		$rc[td_hour]=$ip['summ'];
		$rc[td_12]=getrowval("SELECT SUM(ip_visit) AS `summ` FROM visit_ip WHERE ip_value=".$ip["ip_value"]." AND ip_time>".($xhour-12),'summ');
		$rc[td_20]=getrowval("SELECT SUM(ip_visit) AS `summ` FROM visit_ip WHERE ip_value=".$ip["ip_value"]." AND ip_action>=".$n_min20,'summ');
		$rc[td_5]=getrowval("SELECT SUM(ip_visit) AS `summ` FROM visit_ip WHERE ip_value=".$ip["ip_value"]." AND ip_action>=".$n_min5,'summ');
	}
	
	echo shell_tpl_admin('block/table',$vars);
}

if($section=='settings' && $user->super){
	echo '<h2>'.lng('Settings').'</h2>';
	//echo '<a href="statistics">'.si('back').' вернуться в статистику</a>';
	$vars['url']='statistics';
	$vars['go_back_text']=lng('go back to statistics');
	echo shell_tpl_admin('block/go_back_box',$vars);
	
	if($action=='clean_all'){
		$db->query("DELETE FROM visit_object");
		$db->query("DELETE FROM visit_ip");
		$db->query("DELETE FROM visit_source");
		$db->query("DELETE FROM visit_data");
	}
	
	if($action=='clean_object'){
		$db->query("DELETE FROM visit_object");
	}
	
	if($action=='clean_ip'){
		$db->query("DELETE FROM visit_ip");
	}

	if($action=='clean_source'){
		$db->query("DELETE FROM visit_source");
	}
	
	if($action=='clean_data'){
		$db->query("DELETE FROM visit_data");
		$db->query("DELETE FROM visit_source WHERE source_type=0");
		$db->query("DELETE FROM visit_source WHERE source_type=1");
		$db->query("DELETE FROM visit_object WHERE visit_type=3");
		$db->query("DELETE FROM visit_ip WHERE ip_type=1");
	}
	
	if($action=='clean_left_data'){
		$db->query("DELETE FROM visit_data WHERE 
					(data_type=0 AND (SELECT count(*) FROM visit_source WHERE source_type=0 AND source_domain=data_id)=0) OR
					(data_type=1 AND (SELECT count(*) FROM visit_source WHERE source_type=1 AND (source_domain=data_id OR source_data=data_id))=0) OR
					(data_type=3 AND (SELECT count(*) FROM visit_object WHERE visit_type=3 AND visit_object=data_id)=0) OR
					(data_type=5 AND (SELECT count(*) FROM visit_ip WHERE ip_type=1 AND ip_action=data_id)=0)");
	}
	
	if($action=='clean_object_day'){
		$db->query("DELETE FROM visit_object WHERE visit_type=2 AND visit_period=1");
	}
	
	if($action=='clean_day'){
		clean_day_last(62);
	}
	
	if($action=='clean_hour'){
		clean_hour_last(48);
	}
	
	if($action=='clean_source_last'){
		clean_source_last(62);
	}
	
	if($action=='save'){
		global $del_hour_history, $del_day_history, $del_ip_history, $del_source_history;
		if(empty($del_hour_history2)) $del_hour_history=0; else $del_hour_history=1;
		if(empty($del_day_history2)) $del_day_history=0; else $del_day_history=1;
		if(empty($del_ip_history2)) $del_ip_history=0; else $del_ip_history=1;
		if(empty($del_source_history2)) $del_source_history=0; else $del_source_history=1;
		if(empty($collect_object_history2)) $collect_object_history=0; else $collect_object_history=1;
		if(empty($collect_sources_history2)) $collect_sources_history=0; else $collect_sources_history=1;
		if(empty($ignore_bot2)) $ignore_bot=0; else $ignore_bot=1;
		if(empty($statistics2)) $statistics=0; else $statistics=1;
		save_config();
	}
	
	
	$vars['form_type']='edit';
	$vars['path']='statistics';
	$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'action','value'=>'save');
	$vars['section']['main']['fields'][]=Array('type'=>'hidden','name'=>'section','value'=>'settings');
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'statistics2','value'=>($GLOBALS["statistics"]?"checked":""),'caption'=>lng('Keep statistics'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'del_hour_history2','value'=>($GLOBALS["del_hour_history"]?"checked":""),'caption'=>lng('Automatically clean the hourly history after 48 hours'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'del_day_history2','value'=>($GLOBALS["del_day_history"]?"checked":""),'caption'=>lng('Automatically clear daily history after 62 days'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'del_ip_history2','value'=>($GLOBALS["del_ip_history"]?"checked":""),'caption'=>lng('Automatically clear IP history after 24 hours'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'del_source_history2','value'=>($GLOBALS["del_source_history"]?"checked":""),'caption'=>lng('Automatically clear sources information after 62 days'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'collect_object_history2','value'=>($GLOBALS["collect_object_history"]?"checked":""),'caption'=>lng('Collect statistics of visits objects (pages, news, etc.)'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'collect_sources_history2','value'=>($GLOBALS["collect_sources_history"]?"checked":""),'caption'=>lng('Collect information about the sources of visitors'));
	$vars['section']['main']['fields'][]=Array('type'=>'big_checkbox','name'=>'ignore_bot2','value'=>($GLOBALS["ignore_bot"]?"checked":""),'caption'=>lng('Ignore users whose user-agent differs from browsers agent'));

	echo shell_tpl_admin('block/form', $vars);
	
	echo '<br><br>';
	
	getrow($db,"SELECT count(*) AS cnt FROM visit_object");
	if(!empty($db->Record["cnt"])) echo '<div><a href="statistics?section=settings&action=clean_all">'.lng('Clear all statistics').'</a></div><br>';
	
	getrow($db,"SELECT count(*) AS cnt FROM visit_object");
	if(!empty($db->Record["cnt"])) echo '<div><a href="statistics?section=settings&action=clean_object">'.lng('Clear domain and object statistics').' ('.lng('records').': '.$db->Record["cnt"].')</a></div><br>';
	
	getrow($db,"SELECT count(*) AS cnt FROM visit_ip");
	if(!empty($db->Record["cnt"])) echo '<div><a href="statistics?section=settings&action=clean_ip">'.lng('Clear IP history').' ('.lng('records').': '.$db->Record["cnt"].')</a></div><br>';
	
	getrow($db,"SELECT count(*) AS cnt FROM visit_source");
	if(!empty($db->Record["cnt"])) echo '<div><a href="statistics?section=settings&action=clean_source">'.lng('Clear sources visits').' ('.lng('records').': '.$db->Record["cnt"].')</a></div><br>';
	
	getrow($db,"SELECT count(*) AS cnt FROM visit_data");
	if(!empty($db->Record["cnt"])) echo '<div><a href="statistics?section=settings&action=clean_data">'.lng('Clear text entries').' ('.lng('records').': '.$db->Record["cnt"].', '.lng('ATTENTION - will remove all associated entries statistics - sources, triggers, etc.').')</a></div><br>';
	
	echo '<div><a href="statistics?section=settings&action=clean_left_data">'.lng('Clear unused text entries').'</a></div>';
	echo '<br>';
	
	echo '<div><a href="statistics?section=settings&action=clean_hour">'.lng('Clear statistics "by the hour" (for domains, since the day before yesterday)').'</a></div>';
	echo '<br>';

	echo '<div><a href="statistics?section=settings&action=clean_day">'.lng('Clear statistics "by day" (starting from the month before last)').'</a></div>';
	echo '<br>';
	
	echo '<div><a href="statistics?section=settings&action=clean_source_last">'.lng('Clear the statistics by source (starting with the month before last)').'</a></div>';
	echo '<br>';
	
	echo '<div><a href="statistics?section=settings&action=clean_object_day">'.lng('Clear all statistics "by day" for objects (pages, news, etc.)').'</a></div>';
	echo '<br>';
}

?>