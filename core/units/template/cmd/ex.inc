<?php

function seek_ex($ex){
	global $spex,$db;
	$own=0;
	if(is_object($ex) && isset($ex->id) && !is_numeric($ex->id)) $ex=$ex->id;
	if(!is_object($ex) && !is_numeric($ex) && is_string($ex)){
		if(strstr5($ex,'?')){
			$ex=explode('?',$ex);
			$own=$ex[1];
			$ex=$ex[0];
		}
		if(strstr5($ex,':')){
			$ex=explode(':',$ex);
			$ex=$ex[1];
		}
	}
	if(is_object($ex) && isset($ex->id)){
		if(isset($ex->own)) $own=$ex->own;
		if(isset($ex->owner)) $own=$ex->owner;
		$ex=$ex->id;
	}
	if(empty($ex)) return false;
	if(!isset($spex[$ex])){
		getrow($db,"SELECT * FROM ex_module WHERE ex_id=$ex",1,'ex_module',__LINE__,__FILE__);
		$spex[$ex]=$db->Record;
		global $e2m;
		$e2m[$ex]=$db->Record["ex_module"];
	}
	return Array(	'ex'=>$spex[$ex],
				'own'=>$own,
				'id'=>$ex);
}


$cmd['self'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		$res->id=$ex;
		$res->name=$ed["ex_name"];
		$res->sname=$ed["ex_sname"];
		$res->type='ex';
		return $res;
};
$cmd['self'][CMD_EX]->result=CMD_EX;


$cmd['zone'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $zone_cache_pex,$db;
		if(!isset($zone_cache_pex[$ex])){
			getrow($db,"SELECT * FROM ex_zone WHERE ex_module=$ex AND ex_zone!=0",1,"ex_zone",__LINE__,__FILE__);
			$zone_cache_pex[$ex]=$db->Record;
		}
		return prepend_type($zone_cache_pex[$ex]["ex_zone"],'zn');
};
$cmd['zone'][CMD_EX]->result=CMD_ZONE;
$cmd['zone'][CMD_EX]->visual=Array('title'=>'связанная зона');


$cmd['auto'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];		
		$tmp=shell_module($ed['ex_module'].':'.$ei);
		return $tmp;
};
$cmd['auto'][CMD_EX]->result=CMD_MIXED;
$cmd['auto'][CMD_EX]->result_long=CMD_STRING;
$cmd['auto'][CMD_EX]->visual=Array('title'=>'запуск экземпляра модуля');


$cmd['url_parse'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];		
		$tmp=shell_module($ed['ex_module'].':'.$ex);
		return prepend_type($ex,'ex',Array('own'=>$own));
};
$cmd['url_parse'][CMD_EX]->result=CMD_EX;
$cmd['url_parse'][CMD_EX]->visual=Array('title'=>'запуск обработчика URL');


$cmd['major'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $db;
		$cex=$ex;
		$module=$ed['ex_module'];
		getrow($db,"SELECT * FROM main_table WHERE table_module=$module AND table_bold=1",1,"main_table",__LINE__,__FILE__);
		if(!empty($db->Record)) $ctable=$db->Record["table_id"];
		if(!empty($ctable)) return prepend_type($ctable,'tbl',Array('ex'=>$cex));
		else return false;
};
$cmd['major'][CMD_EX]->result=CMD_TABLE;
$cmd['major'][CMD_EX]->visual=Array('title'=>'Главная таблица');


$cmd['module'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		return prepend_type('md',$ed['ex_module'],Array('ex'=>$ei));
};
$cmd['module'][CMD_EX]->result=CMD_MODULE;
$cmd['module'][CMD_EX]->visual=Array('title'=>'модуль экземпляра');


$cmd['owner'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $ownex, $db, $exzone_cache, $vcache;
		if(isset($ownex[$ex])) return prepend_type($ownex[$ex],'row');
		if(isset($exzone_cache[$ex])) $db->Record=$exzone_cache[$ex];
		else {
			if(empty($ex)) return false;
			getrow($db,"SELECT * FROM ex_zone WHERE ex_module=$ex",1,"ex_zone",__LINE__,__FILE__);
			if(empty($db->Record)){
				if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']!=-1) $db->Record["ex_zone"]=$GLOBALS['zone'][$GLOBALS['za']]['zone_id'];
			}
			$exzone_cache[$ex]=$db->Record;
		}
		if(empty($db->Record["ex_zone"])) return false;
		$zn=$db->Record["ex_zone"];
		$exs=getall($db,"SELECT * FROM ex_zone WHERE ex_zone=$zn AND ex_module!=$ex",1,"ex_zone",__LINE__,__FILE__);
		if(!empty($exs)) foreach($exs AS $tex){
			$cm=$tex["ex_module2"];
			$ce=$tex["ex_module"];
			$cols=getall($db,"SELECT * FROM main_col WHERE col_module=$cm AND col_type=4",1,"main_col",__LINE__,__FILE__);
			if(!empty($cols)){
				$tss='';$tss2=Array();
				foreach($cols AS $rs){
					if(empty($tss2[$rs["col_table"]])){
						if(!empty($tss)) $tss.=',';
						$tss.=$rs["col_table"];
						$tss2[$rs["col_table"]]=1;
						$tcls[$rs["col_id"]]=$rs;
					}
				}
				$tss='('.$tss.')';
				$texs=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$ce AND ex_table IN $tss",1,"ex_group",__LINE__,__FILE__);
				$tss='';
				if(!empty($texs)){
					foreach($texs AS $rs){
						if(!empty($tss)) $tss.=',';
						$tss.=$rs["ex_ex1"];
					}
				}
				$rows='';
				if(!empty($tss)){
					$tss='('.$tss.')';
					$rows=getall($db,"SELECT * FROM main_row WHERE row_module=$cm AND row_ex IN $tss AND row_enable=1",1,"main_row",__LINE__,__FILE__);
				}
				if(!empty($rows)){
					$rss='';
					foreach($rows AS $rs){
						if(!empty($rss)) $rss.=',';
						$rss.=$rs["row_id"];
					}
					$rss='('.$rss.')';
					$css='';
					foreach($cols AS $col){
						if(!empty($css)) $css.=',';
						$css.=$col["col_id"];
					}
					$css='('.$css.')';
					$vals=getall($db,"SELECT * FROM row_value WHERE value_module=$cm AND value_col IN $css AND value_row IN $rss AND value_value!='' AND value_value!=0",1,"row_value",__LINE__,__FILE__);
					if(!empty($vals)) foreach($vals AS $val){
						if(!empty($tcls[$val["value_col"]])){
							$ccol=$tcls[$val["value_col"]];
							if(!empty($ccol["col_force_onshow"])) $val["value_value"]=force_value($val,$ccol);
						}						
						$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
						if(strstr($val["value_value"],':')){
							$tmp=explode(':',$val["value_value"]);
							if($tmp[1]==$ex){
								$ownex[$ex]=$val["value_row"];
								return prepend_type($ownex[$ex],'row',Array('table',$val["value_table"]));
							}
						}
					}
				}
			}
		}
		$ownex[$ex]=0;
		return false;
};
$cmd['owner'][CMD_EX]->result=CMD_ROW;
$cmd['owner'][CMD_EX]->visual=Array('title'=>'объект, к которому присоединён экземпляр');
$cmd['own'][CMD_EX]->alias=&$cmd['owner'][CMD_EX];


$cmd['name'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		return $ed["ex_name"];
};
$cmd['name'][CMD_EX]->result=CMD_STRING;
$cmd['name'][CMD_EX]->visual=Array('title'=>'имя');


$cmd['sname'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		return $ed["ex_sname"];
};
$cmd['sname'][CMD_EX]->result=CMD_STRING;
$cmd['sname'][CMD_EX]->visual=Array('title'=>'спец.имя');


$cmd['tables'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $db;
		$module_id=ex2m($ex);
		if(empty($module_id)) return '';
		
		global $tbl_cache;
		if(!isset($tbl_cache[$module_id])){
			$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$module_id",1,"main_table",__LINE__,__FILE__);
			$tbl_cache[$module_id]=$tbls;
		} else $tbls=$tbl_cache[$module_id];
		$res=Array();$cr=0;
		foreach($tbls AS $tbl)if(check_tbl($tbl["table_id"],'view')){
			$res[$cr]->id=$tbl["table_id"].':'.$ex;
			$res[$cr]->name=$tbl["table_name"];
			$res[$cr]->sname=$tbl["table_sname"];
			$res[$cr]->owner=0;
			$res[$cr]->tex=get_tex(0,$ex,$tbl["table_id"]);
			$res[$cr]->type='tbl';
			$cr++;
		}
		return $res;
};
$cmd['tables'][CMD_EX]->result=CMD_ARRAY;
$cmd['tables'][CMD_EX]->result_long=CMD_TABLE;
$cmd['tables'][CMD_EX]->visual=Array('title'=>'таблицы');


$cmd['part'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		$ajax=0;
		$module=$ed["ex_module"];
		if(shift_first_op($ops,'ajax')) $ajax=1;
		if(shift_first_op($ops,'ajax2')) $ajax=2;
		$part_sname=$ops[key($ops)]->data;
		$op=get_first_op($ops);
		global $cmd;
		if(isset($ops[key($ops)]) && $ops[key($ops)]->type==LEXER_CMD && !empty($ops[key($ops)]->data) && isset($cmd[$ops[key($ops)]->data][CMD_PART])){
			$part_id=getrowval("SELECT part_id, part_sname, part_module FROM main_part WHERE part_module=$module AND part_sname='".$part_sname."'","part_id");
			return prepend_type($part_id,'prt',Array('module'=>$module, 'ex'=>$ex));
		}
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'',$ajax,$module,$ex,$part_sname);
};
$cmd['part'][CMD_EX]->result=CMD_MIXED;
$cmd['part'][CMD_EX]->visual=Array('title'=>'части','special'=>SP_PART,'disable_insert'=>1,'result'=>CMD_PART,'pos'=>/*3*/1);


$cmd['url_part'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		//$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$module=$ed['ex_module'];
		$mdata=$module.':'.$ex;
		$part_sname=$ops[key($ops)]->data;
		$op=get_first_op($ops);
		if(getrowval("SELECT part_id FROM main_part WHERE part_module=$module AND part_sname='$part_sname'","part_id")) return shell_module($mdata,$part_sname,0,$op);
		else return false;
};
$cmd['url_part'][CMD_EX]->result=CMD_MIXED;
$cmd['url_part'][CMD_EX]->visual=Array('title'=>'вызвать часть с определением объекта по URL','special'=>SP_POINT,'put'=>Array(
		0=>Array('title'=>'Часть','type'=>STRING_PART_NAME,'req'=>1)
));


$cmd['sel'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $url_row3;
		if(isset($url_row3[$ex])){
			$tbl=$url_row3[$ex];
			return prepend_type($tbl,'tbl',Array('ex'=>$ex, 'own'=>$own));
		} else return false;
};
$cmd['sel'][CMD_EX]->result=CMD_TABLE;
$cmd['sel_table'][CMD_EX]->alias=&$cmd['sel'][CMD_EX];
$cmd['sel'][CMD_EX]->visual=Array('title'=>'текущая таблица');


$cmd['param'][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $pex,$prow2,$db;
		if(!isset($pex[$ex]) || !isset($prow2[-$ex])) prepend_ex($ex);
		$lm=$GLOBALS["cur_module"];
		$op=get_first_op($ops);
		$var[1]=$op->data;		
		
		if(!isset($pex[$ex]->param[$var[1]][$lm])) $lm=ex2m($ex);
		
		if(!isset($pex[$ex]->param[$var[1]][$lm]) && isset($pex[$ex]->param[$var[1]])) foreach($pex[$ex]->param[$var[1]] AS $var=>$value){ $lm=$var; break;}
		//это рискованный ход. дело в том, что мы не знаем точно о каком модуле идёт речь, если обрабатывается экземпляр подтаблицы, т.к. экземпляр у подтаблицы и таблицы общий, а значит определять по нему модуль некорректно. а вот по чему тогда определять ID модуля не ясно
		//вообще можно выбрать первую попавшуюся module в pex->param[name][module]	
		
	 	// Поиск перекрытия переменных экземпляра (нуждается в оптимизации)		
	 	// +10 запросов к простому рендерингу - много
	 	
		if($GLOBALS['zone'][$GLOBALS['za']]['zone_module']==-1){
			if(!isset($GLOBALS["id6"]) && isset($_GET["id6"])) $GLOBALS["id6"]=$_GET["id6"];
			if(!isset($GLOBALS["id6"]) && isset($_POST["id6"])) $GLOBALS["id6"]=$_POST["id6"];
			$owner_elem=$GLOBALS["id6"];
			if(isset($GLOBALS["cur_row"])) $owner_elem=$GLOBALS["cur_row"];
		} else {
			if(isset($GLOBALS["cur_row"])){
				$owner_elem=$GLOBALS["cur_row"];
			}
		}
		if(!empty($owner_elem) && !empty($prow2[-$ex][$var[1]])){
			global $cex_overlay_cache,$rlink;
			$col_sname=$var[1];
			$col=$prow2[-$ex][$var[1]];
			if($col["col_module"]!=$lm) $col=getrow($db,"SELECT * FROM main_col WHERE col_module=$lm AND col_sname='$col_sname' AND col_table=0");
			if(!isset($cex_overlay_cache[$col_sname][$lm][$owner_elem])){
				$cex_overlay_cache[$col_sname][$lm][$owner_elem]=false;
				seek_rlink($owner_elem);
				//$ownrs=Array($owner_elem);
				if($GLOBALS['cur_module']!=$rlink[$owner_elem]->module) $ownrs=Array($owner_elem);
				else $ownrs=Array();
				$rl=$rlink[$owner_elem];
				while($rl->owner!=0){
					if($rl->owner!=$owner_elem) $ownrs[]=$rl->owner;//тут нельзя ставить индекс
					seek_rlink($rl->owner);
					$rl=$rlink[$rl->owner];
				}
				if(!empty($ownrs)){
					$res=getall6($db,"SELECT * FROM row_value WHERE value_row IN (".implode(',',$ownrs).") AND value_table!=0 AND value_col=".$col["col_id"],"value_row","value_value");
					if(!empty($res)) foreach($ownrs AS $o) if(isset($res[$o])){
						$cex_overlay_cache[$col_sname][$lm][$owner_elem]=$res[$o];
						break;
					}
					//if(count($res)==0){
					//	$cex_overlay_cache[$col_sname][$lm][$owner_elem]=false;
					//}
				}
			}
			if(!($cex_overlay_cache[$col_sname][$lm][$owner_elem]===false)){
				$row2_overlay=$cex_overlay_cache[$col_sname][$lm][$owner_elem];
			}
		}
		
		if((isset($pex[$ex]->param[$var[1]][$lm]) || isset($row2_overlay)) && !empty($prow2[-$ex][$var[1]])){
			$col=$prow2[-$ex][$var[1]];
			if(isset($row2_overlay)) $row2=$row2_overlay;
			else $row2=$pex[$ex]->param[$var[1]][$lm];
			if(!$row2) return false;
			if($col["col_type"]==1 && !is_array($row2) && (!is_object($row2) || is_object($row2) && !isset($row2->rows))){
				return prepend_type($row2,'row');
			}
			else if($col["col_type"]==4)  return prepend_type($row2,'ex');
			else if($col["col_type"]==5)  return prepend_type($row2,'usr');
			else {
				return $row2;
			}
		}
};
$cmd['param'][CMD_EX]->result=CMD_MIXED;
$cmd['param'][CMD_EX]->visual=Array('title'=>'переменные'/*,'put'=>Array(
		0=>Array('title'=>'Название переменной','type'=>STRING_EX_PARAM,'req'=>1)
)*/,'special'=>SP_EX_PARAM,'object_type'=>OB_MODULE,'disable_insert'=>1,'pos'=>0);


$cmd[''][CMD_EX]->process=function(&$data,&$op,&$ops){
		$e=seek_ex($data); $own=$e['own']; $ed=$e['ex']; $ei=$e['id']; $ex=$e['id'];
		global $db;
		$var[0]=$op->data;
		/*if($var[0]=='comments'){
			echo $own;
			var_dump($GLOBALS["cur_ex"]);
			echo '<br><br>';
		}*/
		if(empty($own)){
			$module_id=ex2m($ex);
			$ctable=check_table($var[0],$module_id);
		} else {
			global $pcol_tbl;
			if(!isset($pcol_tbl[$own][$var[0]])){
				getrow($db,"SELECT * FROM main_row WHERE row_id=$own",1,"main_row",__LINE__,__FILE__);
				if(!empty($db->Record)) $tmp4=getall($db,"SELECT * FROM table_sub WHERE sub_table1=".$db->Record["row_table"],1,"table_sub",__LINE__,__FILE__);
				$tmp3=Array();
				if(!empty($tmp4)) foreach($tmp4 AS $tmp5) $tmp3[]=$tmp5["sub_table2"];
				getrow($db,"SELECT * FROM main_table WHERE table_id IN (".implode(',',$tmp3).") AND table_sname='$var[0]'",1,"main_table",__LINE__,__FILE__);
				if(!empty($db->Record)){
					$ctable=$db->Record["table_id"];
					$pcol_tbl[$own][$var[0]]=$ctable;
				} else $pcol_tbl[$own][$var[0]]=0;
				$ex=0;
			} else {
				$ctable=$pcol_tbl[$own][$var[0]];
				$ex=0;
			}
		}

		if(!empty($ctable)){
			return prepend_type($ctable,'tbl',Array('ex'=>$ex, 'own'=>$own));
		} else {
			/*if(getrowval("SELECT part_sname FROM main_part WHERE part_module=".$ed['ex_module']." AND part_sname='$op->data'","part_sname")){
				$x=get_cmd_link('part',CMD_EX);
				$nop=new lexer_op();
				$nop->next_operation='.';
				$nop->data='part';
				array_unshift($ops,$op);
				array_unshift($ops,$nop);
				return $x($data,$op,$ops);
			}*/
		}
};
$cmd[''][CMD_EX]->result=CMD_TABLE;
$cmd[''][CMD_EX]->visual=Array('title'=>'','special'=>SP_UNKNOWN,'object_type'=>OB_MODULE,'pos'=>/*1*/2);

?>