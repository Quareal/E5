<?php
	global $db,$tqrow,$debug2,$fcount,$aquery;
	
	function rcount ($array) {
		 $count = 0;
		 if (is_array($array))  foreach($array as $id=>$sub) {
			 if (!is_array($sub))  $count++;
			 else $count = ($count + rcount($sub));
		 }
		 return $count;
	 }

	$mgu=memory_get_usage();
	if(function_exists('memory_get_peak_usage')) $mgpu=memory_get_peak_usage();
	
	if($GLOBALS["zone"][$GLOBALS["za"]]["zone_module"]==-1){
		$debug_top='<a name="adebug"></a><div align="right" style="font-family: Verdana; font-size: 11px; padding: 10px; border-top: 1px solid #000000;">';
		$debug_pad=0;
	} else {
		$debug_top='<div class="uplink" align="right" style="font-family: Verdana; font-size: 11px; padding-right: 50px; margin-top: 100px; padding-top: 10px; border-top: 1px solid #000000;">';
		$debug_pad=250;
	}
	
	echo '<div style="background-color: #FFFFFF; color: #000000;">';
	$ds='
		<style>
			.link{color: #0000FF;cursor: pointer;}
			.link:hover{color: #0000FF;cursor: pointer;text-decoration: underline;}
		</style>
		<script>function showhideD(id){$(\'#\'+id).toggle();}</script>
		'.$debug_top.'
		Запросов к базе: <span class="link" OnClick="showhideD(\'query_list\');">';		
	 global $tquery;$ds.=$tquery; 
	$ds.='</span>
		<div align="left" id="query_list" style="padding-left: '.$debug_pad.'px; display: none;">';
			$ds.='<div> Строк обработано: '.$tqrow.' (ниже расшифровка t - время, n - кол-во затронутых рядов)</div>';
			$valc=0;
			$aaq=Array();
			if(!empty($aquery)) foreach($aquery AS $aq){
				if(strstr($aq->s,'* FROM row_value')) $valc++;
				if(!empty($aaq[$aq->s])) $ds.='<span style="color:#FF0000;">';
				if(!empty($aaq[$aq->s])) $aq->t=0;
				$ds.=$aq->s;
				if($aq->y==2) $ds.='<span style="color: #FF0000;">!</span>';
				if(!empty($aq->s) && !empty($aq->z) && !strstr($aq->s,$aq->z)) $ds.='<span style="color: #FF0000;">! : '.$aq->z.'</span>';
				$ds.=' (t='.$aq->t.';n=';
				if($aq->n>200) $ds.='<span style="color: #FF0000;">';
				$ds.=$aq->n;
				if($aq->n>200) $ds.='</span>';
				$ds.=')';
				if(!empty($aaq[$aq->s])) $ds.='</span>';
				$ds.=' <span style="color: #FFFFFF;">/ '.$aq->f.' : '.$aq->l.'</span>';
				$ds.='<br>';$aaq[$aq->s]=1;
			}
			$ds.='<br><br>Количество важных/второстепенных запросов: <b>'.($tquery-$valc).' / '.$valc.'</b> (оптимизируйте второстепенные запросы)';
			global $pdebug; $GLOBALS["debug"]=false; $prts2=getall($db,"SELECT * FROM main_part");$prts=Array();	if(!empty($prts2)) foreach($prts2 AS $p) $prts[$p["part_id"]]=$p["part_name"]; unset($prts2);
			if(!empty($pdebug)){
				$ds.='<br><br><b>Разбор по частям</b><br>';
				foreach($pdebug AS $p=>$pri)if(isset($prts[$p])){
					$ds.='<br>Часть "<b>'.$prts[$p].'</b> (кол-во запросов: '.count($pri->aqs).' | время: '.$pri->time.' | кол-во затронутых рядов: '.$pri->num.')"<br>';
					$aaq=Array();
					foreach($pri->aqs AS $aq){
						if(!empty($aaq[$aq->s])) $ds.='<span style="color:#FF0000;">';
						if(!empty($aaq[$aq->s])) $aq->t=0;
						$ds.=$aq->s.' (t='.$aq->t.';n=';
						if($aq->n>200) $ds.='<span style="color: #FF0000;">';
						$ds.=$aq->n;
						if($aq->n>200) $ds.='</span>';
						$ds.=')';
						if(!empty($aaq[$aq->s])) $ds.='</span>';
						if(isset($aq->f)) $ds.=' <span style="color: #FFFFFF;">/ '.$aq->f.' : '.$aq->l.'</span>';
						$ds.='<br>';$aaq[$aq->s]=1;
					}
				}
			}
			global $bquery,$cquery,$dquery;
			if(!empty($bquery)){
				$bquery=array_flip($bquery);
				krsort($bquery);
				$bquery=array_flip($bquery);
				$ds.='<br><br>Тайминг запросов по строкам (время в 1/100000сек, кол-во вызовов, кол-во строк):<br>';
				foreach($bquery AS $line=>$time){
					$ds.=$line.'  ('.$time.', '.$cquery[$line].', '.$dquery[$line].')<br>';
				}				
			}
		$ds.='</div>
		<br>Общее время: ';
		global $start_time,$end_time;
		if(empty($end_time)){
			$mtime = microtime();$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];$end_time = $mtime;
		}
		$total_time = ($end_time - $start_time);
		$ds.=substr($total_time,0,5);
		$ttime=$total_time;
		$ds.='<br>Память (кб): '.floor($mgu/1024);
		if(function_exists('memory_get_peak_usage')) $ds.=' / '.floor($mgpu/1024);
		$ds.='<br><br>';
		$ds.='<span class="link" OnClick="showhideD(\'server_panel\')">Сервер</span>, ';
		$ds.='<span class="link" OnClick="showhideD(\'global_panel\');">Глобальные</span>, ';
		$ds.='<span class="link" OnClick="showhideD(\'func_panel\');">Функции</span>';
		//$ds.='<span class="link" OnClick="showhideD(\'code_panel\');">Код</span>';
		$ds.='<div id="func_panel" style="display: none; padding-left: '.$debug_pad.'px;" align="left">';
		$ds.='<b>Тайминг</b>. Команда = Время (кол-во вызовов/кол-во SQL запросов/кол-во затронутых рядов):<br>';
	
		if(isset($GLOBALS['init_timer'])){
			$ds.='initialization = '.substr($GLOBALS['init_timer'],0,5).'<br>';
		}
		global $equery;
		$ttime2=0;
		if(!empty($GLOBALS["ftime"])){
			global $ftime4;
			$mid=0;
			foreach($GLOBALS["ftime"] AS $var=>$value) $mid=$mid+$value->time;
			$mid2=$mid;
			if(count($GLOBALS["ftime"])>0) $mid=$mid/count($GLOBALS["ftime"]); else $mid=0;
			$co=0;
			foreach($GLOBALS["ftime"] AS $var=>$value)if($value->time>$mid){
				$co++;
			}
			if($co!=0) 	$mad=$mid2/$co; else $mad=0;
			$k=0;
			
			$resort=Array();
			foreach($GLOBALS["ftime"] AS $var=>$value) $resort[substr($value->time,0,5)][]=$var;
			krsort($resort);			
			//foreach($GLOBALS["ftime"] AS $var=>$value){
			foreach($resort AS $time=>$arr) foreach($arr AS $a){
				$var=$a;
				$value=$GLOBALS['ftime'][$a];
				$k++;
				$ttime2+=$value->time;
				if($value->time>$mad) $ds.='<div style="color: #FF0000;">'.$var.' = '.substr($value->time,0,5).' ('.$fcount[$var].' / '.$value->query.' / '.$value->row.') <span style="color: #AAAAAA;">'.$ftime4[$var].'</span>';
				else $ds.='<div>'.$var.' = '.substr($value->time,0,5). ' ('.$fcount[$var].' / '.$value->query.' / '.$value->row.') <span style="color: #AAAAAA;">'.$ftime4[$var].'</span>';
				if(!empty($equery[$var]))	$ds.=' <span class="link" OnClick="showhideD(\'fn'.$k.'\')">sql</span>';
				$ds.='</div>';

				if(!empty($equery[$var])){
					$ds.='<div style="padding-left: 20px; padding-bottom: 10px; padding-top: 5px; display: none;" id="fn'.$k.'"> Запрос (кол-во вызовов / время / кол-во рядов)<br>';
					foreach($equery[$var] AS $sql=>$e){
						$ds.='<span style="color: #999999;">'.$sql.'</span> ('.$e->count.' / '.$e->time.' / '.$e->num.')<br>';
					}
					$ds.='</div>';
				}
				
			}
		}
		$ds.='<br>Остальные функции: '.substr($ttime-$ttime2,0,5).'<br>';
		
		/*$ds.='<br><b>Порядок выполнения частей:</b><br>';
		global $log1;
		if(!empty($log1)) foreach($log1 AS $l){
			$ds.=$l->t.'<br>';
		}
		$ds.='<br><br>&nbsp;';
	
			$ds.='<br><br><b>Примерная вложенность</b>: <br>';global $fproc; $ds.=$fproc;*/
		$ds.='</div><div id="server_panel" style="display: none; padding-left: '.$debug_pad.'px;" align="left">';
		foreach($_SERVER as $var=>$value)if(!empty($value)) $ds.=$var.' = '.$value.'<br>';
		$ds.='PHP VERSION = '.PHP_VERSION.'<br>';
		$ds.='</div><div id="global_panel" style="display: none; padding-left: '.$debug_pad.'px;" align="left">';
		if($debug2){
			$gl=Array();
			foreach($GLOBALS AS $var=>$value)if($var!='GLOBALS' && $var!='gl' && $var!='ds' && $var!='prts'){
				if(!is_array($value)) $i2=1; else $i2=count($value);
				//$i=strlen(/*serialize*/json_encode($value));
				$i=0;
				$r=Array();
				$r["name"]=$var;
				$r["count"]=$i2;
				$r["size"]=$i;
				$gl[rcount($value)][]=$r;
			}
			krsort($gl);
			$ds.='Кол-во элементов - название массива (кол-во элементов 1ого уровня, длинна serialize)<br>';
			foreach($gl as $count=>$value)foreach($value AS $n){
				if($n["size"]>100000) $dr='<span style="color: #990000;">'; else $dr='<span>';  
				$ds.='<span style="color: #AAAAAA;">'.$count.'</span> - '.$n["name"].' ('.$n["count"].' / '.$dr.$n["size"].'</span>)<br>';
			}
		} else $ds.='Для отображения глобальных переменных включите расширенную статистику';
		$ds.='</div>';
		
		/*$ds.='<div id="code_panel" style="display: none; padding-left: '.$debug_pad.'px;" align="left">';
		if($debug2){
			global $ptime, $pcount;
			$ptime_arr=Array();
			if(!empty($ptime)) foreach($ptime AS $code=>$pt){
				$ptime_arr[$pt->time*10000][]=$code;
			}
			krsort($ptime_arr);
			if(empty($ptime_arr)) $ds.='На протяжении данной сессии код Е5 не исполнялся<br><br><br>&nbsp';
			foreach($ptime_arr AS $time=>$tmp) foreach($tmp AS $code){
				$ds.='<div>'.($time/10000).' : '.$pcount[$code].' - '.$code.'</div>';
			}
		} else $ds.='Для отображения трейса по выполненным командам Е5 включите расширенную статистику';
		$ds.='</div>';*/
		
		$ds.='</div>';
		echo $ds;
		if(!empty($GLOBALS["testd"])) echo $GLOBALS["testd"];
		echo '</div>';

?>