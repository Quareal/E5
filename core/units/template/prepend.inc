<?php

function prepend_param($pv,$part_major=0,$part_id=0,$flush=0){
	global $p_par,$db,$pparam;
	//debug_wayback();
	$par=$p_par;
	//if(empty($part_id)) return $par; //и такое бывает
	$p_par2=Array();
	stf('prepend_param');
	//$p_par=Array();		//это закомментированно специально для проверки. чтобы несколько частей могли юзать переменные друг-друга
	if(!empty($pv)) foreach($pv AS $ops){		
		if(is_array($ops)){
			//$name=array_getA($ops,0)->data;
			//$val=parse_var(array_getA($ops,1));
			//$name=$ops[key($ops)]->data;
			//$val=parse_var($ops[key($ops)+1]);
			if(!isset($ops[0]->data) && !empty($ops[0]->sub)){
				//new section
				$name=$ops[0]->sub[0]->data;
				$val=parse_var($ops[0]->sub[1]);
			} else {
				$name=$ops[0]->data;
				$val=parse_var($ops[1]);
			}
		} else {
			$name=$ops->sub[0]->data;
			$val=parse_var($ops->sub[1]);
		}
		$p_par2[$name]=$val;
		$pv2[$name]=1;
		if(!isset($p_par[$name])){
			//if($part_id==93 || $part_id==95) echo $name.'<br>';
 			$p_par[$name]='';
			$par[$name]='';
		}
	}
	if(!empty($part_major)){
		if($part_major!=0 && !empty($GLOBALS["cur_col"])){
			global $param_cache2;
			if(!isset($param_cache2[$part_major])){
				getrow($db,"SELECT param_id, param_sname, param_type FROM part_param WHERE param_id=$part_major",1,"part_param",__LINE__,__FILE__);
				$param_cache2[$part_major]=$db->Record;
			}
			$mparam=$param_cache2[$part_major];
			if(!empty($mparam) && !isset($pv2[$mparam["param_sname"]])){
				if($mparam['param_type']==0 && !empty($GLOBALS["cur_row"]) && !empty($GLOBALS['cur_col']->id)) $p_par2[$mparam["param_sname"]]=rowcol_val($GLOBALS["cur_row"], getrow($db,"SELECT col_id, col_force_onshow, col_sname FROM main_col WHERE col_id=".$GLOBALS['cur_col']->id), 0);
				else $p_par2[$mparam["param_sname"]]=$GLOBALS["cur_col"];
			}
		}
	}
	if(!empty($part_id)){
		global $param_cache3;
		if(!isset($param_cache3[$part_id])){
			$param_cache3[$part_id]=getall($db,"SELECT * FROM part_param WHERE param_part=$part_id",1,"part_param",__LINE__,__FILE__);
		}
		$pa=$param_cache3[$part_id];
		if(!empty($pa)) foreach($pa AS $p){
			$pparam[$p["param_part"]][$p["param_sname"]]=$p;
			if(isset($p_par2[$p["param_sname"]]) && $p['param_array'] && !is_array($p_par2[$p["param_sname"]])) $p_par2[$p["param_sname"]]=Array($p_par2[$p["param_sname"]]);
			if(!isset($p_par2[$p["param_sname"]])){
				if($p["param_get"]==0 || (empty($_GET[$p["param_sname"]]) && empty($_POST[$p["param_sname"]]) )){
					$p_par2[$p["param_sname"]]=$p["param_default"];
				} else {
					if(!empty($_GET[$p["param_sname"]])) $p_par2[$p["param_sname"]]=urldecode($_GET[$p["param_sname"]]);
					if(!empty($_POST[$p["param_sname"]])) $p_par2[$p["param_sname"]]=$_POST[$p["param_sname"]];
				}
			}
		}
	}
	if($flush) $p_par=$p_par2;
	else foreach($p_par2 AS $var=>$value) $p_par[$var]=$value;
	etf('prepend_param');
	//if($part_id==93 || $part_id==95) echo var_dump($par['block_tpl']);
	return $par;
}

function prepend_part($module){
	global $db,$ppart;
	stf('prepend_part');
	$tmp=getall($db,"SELECT part_id,part_name,part_url,part_module,part_major,part_sname FROM main_part WHERE part_module=$module",1,'main_part',__LINE__,__FILE__);
	foreach($tmp AS $tm){
		//$GLOBALS["part_cache4"][$tm["part_id"]]=$tm;
		$ppart[$module][$tm["part_sname"]]->id=$tm["part_id"];
		$ppart[$module][$tm["part_sname"]]->url=$tm["part_url"];
		$ppart[$module][$tm["part_sname"]]->name=$tm["part_name"];
		$ppart[$module][$tm["part_sname"]]->major=$tm["part_major"];
	}
	etf('prepend_part');
}

function prepend_row($row){
	global $db,$prow,$pccol,$globr,$rlink,$rlink2,$prow2,$pccol2,$vcache,$vcache2,$no_cache;
	if($no_cache){ unset($vcache); unset($vcache2);}
	stf('prepend_row');
	do_log('Prepend row '.$row);
	seek_rlink($row);
	if($rlink[$row]->enable==0 || $rlink[$row]->enable==-1){etf('prepend_row'); return '';}
	if($rlink2 && $GLOBALS["use_globr"]){
		$st='';
		foreach($rlink AS $var=>$value){
			if(!empty($st)) $st.=',';
			$st.=$var;
		}
		$tmp=getall($db,"SELECT * FROM row_value WHERE value_row IN ($st) AND value_table>0",1,'row_value'/*,__LINE__,__FILE__*/);
		foreach($tmp AS $tm){
			$globr[$tm["value_row"]][]=$tm;
			$vcache[$tm["value_col"]][$tm["value_row"]][$tm["value_id"]]=$tm;
			$vcache2[$tm["value_col"]][$tm["value_row"]]=$tm;
		}
		$rlink2=false;		
	}
	global $vcache2;
	if(!empty($globr[$row])) $vals=$globr[$row];
	// GLOBR - кеширование, которое собирает также информацию из неиспользованных столбцов
	else {
		//101 (101 - код ахтунга) АХТУНГ! Это отключение сделано просто для оптимизации ряда запросов. Неизвестно к чему это приведёт!
		$vals=getall($db,"SELECT * FROM row_value WHERE value_row=$row AND value_table>0",1,'row_value'/*,__LINE__,__FILE__*/);
		$globr[$row]=$vals;//меня смущает что раньше этого небыло
	}
	$ccol='';$ccol2=Array();$cls=Array();
	if(!empty($vals)) foreach($vals AS $val){
		$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
		$vcache2[$val["value_col"]][$val["value_row"]]=$val;
		if(empty($ccol2[$val["value_col"]])){
			if(empty($pccol[$val["value_col"]])){
				if(!empty($ccol)) $ccol.=',';
				$ccol.=$val["value_col"];
			} else $cls[$val["value_col"]]=$pccol[$val["value_col"]];
			$ccol2[$val["value_col"]]=1;
		}
	}
	if(!empty($ccol)){
		$cols=getall($db,"SELECT * FROM main_col WHERE col_id IN ($ccol)",1,'main_col',__LINE__,__FILE__);
		if(!empty($cols)) foreach($cols AS $col){
			$pccol[$col["col_id"]]=$col;
			$pccol2[$col["col_module"]][$col["col_table"]][$col["col_sname"]]=$col;
			$cls[$col["col_id"]]=$col;
		}
	}
	$cow=Array();
	if(!empty($vals)) foreach($vals AS $val)if(!empty($val['value_col'])){
		$ccol=$cls[$val["value_col"]];
		if(!empty($ccol["col_force_onshow"])) $val["value_value"]=force_value($val,$ccol);
		if($ccol["col_link2"]==0){
			$prow[$row][$ccol["col_sname"]]=$val["value_value"];
			$prow2[$row][$ccol["col_sname"]]=$ccol;
		} else {
			$prow2[$row][$ccol["col_sname"]]=$ccol;
			seek_rlink($val["value_value"]);
			if(isset($rlink[$val["value_value"]])){
				if($rlink[$val["value_value"]]->enable!=-1 && $rlink[$val["value_value"]]->enable==1){
					if(empty($prow[$row][$ccol["col_sname"]])) $prow[$row][$ccol["col_sname"]]=Array();
					$prow[$row][$ccol["col_sname"]][]->id=$val["value_value"];
				}
			}
		}
	}
	etf('prepend_row');
}

function prepend_ex($ex){
	global $db,$pex,$pvrow,$prow2,$vcache;
	stf('prepend_ex');
	do_log('Prepend ex '.$ex);
	if(empty($ex)){etf('prepend_ex'); return '';}
	if(empty($pvrow[$ex])){
		$vals=getall($db,"SELECT * FROM row_value WHERE value_row=$ex AND value_table=0",1,'row_value',__LINE__,__FILE__);
		$pvrow[$ex]=$vals;
	}else $vals=$pvrow[$ex];
	$ccol='';$ccol2=Array();
	if(!empty($vals)) foreach($vals AS $val){
		$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
		if(empty($ccol2[$val["value_col"]])){
			if(!empty($ccol)) $ccol.=',';
			$ccol.=$val["value_col"];
			$ccol2[$val["value_col"]]=1;
		}
	}
	if(!empty($ccol)){
		$cols=getall($db,"SELECT * FROM main_col WHERE col_id IN ($ccol)",1,'main_col',__LINE__,__FILE__);
		$cls=Array();
		if(!empty($cols)) foreach($cols AS $col)$cls[$col["col_id"]]=$col;
	} else $cols=Array();
	$cow=Array();
	if(!empty($vals)) foreach($vals AS $val){
		$ccol=$cls[$val["value_col"]];
		if(!empty($ccol["col_force_onshow"])) $val["value_value"]=force_value($val,$ccol);
		if($ccol["col_link2"]==0){
			if(empty($pex[$ex]->param[$ccol["col_sname"]][$ccol["col_module"]]))	//ввёл эту строчку и начала отображаться переменная cex.param.tpl у ГБ... чё к чему? непонятно... ещё и группы зон с экземплярами модулей на атомике куда-то делись...
				$pex[$ex]->param[$ccol["col_sname"]][$ccol["col_module"]]=$val["value_value"];
			$prow2[-$ex][$ccol["col_sname"]]=$ccol;
		} else {
			$prow2[-$ex][$ccol["col_sname"]]=$ccol;
			if(!isset($pex[$ex]->param[$ccol["col_sname"]][$ccol["col_module"]])) $pex[$ex]->param[$ccol["col_sname"]][$ccol["col_module"]]=Array();
			$pex[$ex]->param[$ccol["col_sname"]][$ccol["col_module"]][]->id=$val["value_value"];
		}
	}
	etf('prepend_ex');
}

?>