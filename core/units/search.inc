<?php

function start_auto_index($limit=0){
	global $db, $max_new_lemma_at_once, $new_lemma;
	if($limit==0){
		$limit=$GLOBALS["index_at_once"];
	}
	$total_lemma_added=0;
	$rowcols=getall($db,"SELECT index_row, index_col, index_word FROM row_index WHERE index_word=0 LIMIT $limit",1,"row_index");
	if(empty($rowcols)) return;
	//$db->query("DELETE FROM row_index WHERE index_word=0 LIMIT $limit",3,"row_index");
	$query=Array();
	$rs=Array();
	foreach($rowcols AS $rowcol){
		$rs[$rowcol["index_row"]]=$rowcol["index_row"];
		if(empty($query[$rowcol["index_col"]])) $query[$rowcol["index_col"]]=$rowcol["index_row"]; else $query[$rowcol["index_col"]].=','.$rowcol["index_row"];
	}
	$rs=implode(',',$rs);
	$rs=getall($db,"SELECT ro_ex, owner_id, row_id FROM row_owner WHERE row_id IN ($rs)",1,"row_owner");
	if(empty($rs)) return;
	$rws=Array();
	foreach($rs AS $r){
		$rws[$r["row_id"]][]=$r;
	}
	//$del_index=Array();
	$group_by=100;
	$group_cur=0;
	$group_sql=Array();
	$group_sql[$group_cur]=Array();	
	foreach($query AS $col=>$rows){
		$vals=getall($db,"SELECT * FROM row_value WHERE value_col=$col AND value_row IN ($rows) AND value_value!=''",1,"row_value");
		if(!empty($vals)) foreach($vals AS $val){
			$row=$val["value_row"];
			$val=$val["value_value"];
			//$del_index[$col][$row]=$row;
			$words=text2index($val,1);
			if(!empty($words)) foreach($words AS $word){
				if(!empty($rws[$row])) foreach($rws[$row] AS $cr){
					if(count($group_sql[$group_cur])>=$group_by) $group_cur++;
					$group_sql[$group_cur][]="(".$word->lemma.", ".$word->pos.", $row, $col, ".$cr["ro_ex"].", ".$cr["owner_id"].")";					
					//$db->query("INSERT INTO row_index (index_word, index_pos, index_row, index_col, index_tex, index_owner)
					//		VALUES (".$word->lemma.", ".$word->pos.", $row, $col, ".$cr["ro_ex"].", ".$cr["owner_id"].")",3,"row_index");
				}
			}
			$total_lemma_added+=$new_lemma;
			if($total_lemma_added>$max_new_lemma_at_once) break;
		}
		if($total_lemma_added>$max_new_lemma_at_once) break;
	}
	foreach($query AS $col=>$rows){
		$db->query("DELETE FROM row_index WHERE index_word=0 AND index_col=$col AND index_row IN ($rows)",3,"row_index");
	}
	//foreach($del_index AS $col=>$tmp){
	//	$rows=implode(',',$tmp);
	//	$db->query("DELETE FROM row_index WHERE index_word=0 AND index_col=$col AND index_row IN ($rows)",3,"row_index");
	//}
	foreach($group_sql AS $group)if(!empty($group)){
		$x=implode(',
',$group);
		if(!empty($x)) $db->query("INSERT INTO row_index (index_word, index_pos, index_row, index_col, index_tex, index_owner) VALUES
".$x,3,"row_index");
	}
}

function add_job_col($col){
	global $db;
	getrow($db,"SELECT * FROM main_col WHERE col_id=$col",1,"main_col");
	if(empty($db->Record)) return;	
	$rows=getall($db,"SELECT row_id,row_table,row_module FROM main_row WHERE row_table=".$db->Record["col_table"]." AND row_module=".$db->Record["col_module"],1,"main_row");
	
	$max_group=500;
	$group=0;
	$sql=Array();
	$sql[$group]=Array();
	foreach($rows AS $row){
		if(count($sql[$group])>=$max_group){
			$group++;
			$sql[$group]=Array();
		}
		$sql[$group][]="(0, ".$row["row_id"].", $col)";
	}
	foreach($sql AS $arr){
		$x=implode(',
',$arr);
	if(!empty($x)) $db->query("INSERT INTO row_index (index_word, index_row, index_col) VALUES
".$x,3,"row_index");
	}
	
	//foreach($rows AS $row){	
	//	$db->query("INSERT INTO row_index (index_word, index_row, index_col) VALUES (-1, ".$row["row_id"].", $col)",3,"row_index");
	//}
}

function remove_col_index($col){
	global $db;
	//@$db->query("SET group_concat_max_len = -1");			
	//getrow($db,"SELECT count(index_word) AS cnt FROM (SELECT index_word FROM row_index WHERE index_col=$col GROUP BY index_word) t1");
	//$cnt=$db->Record["cnt"];
	//getrow($db,"SELECT GROUP_CONCAT(index_word SEPARATOR ',') AS data FROM (SELECT index_word FROM row_index WHERE index_col=$col GROUP BY index_word) t1");
	//$words=explode(',',$db->Record["data"]);
	//if($cnt>count($words)){
	//	$words=getall($db,"SELECT index_word FROM row_index WHERE index_col=$col GROUP BY index_word",1,"index_word");
	//}
	$db->query("DELETE FROM row_index WHERE index_col=$col",3,"row_index");	
	//if(!empty($words)) $words2=getall3($db,"SELECT index_word FROM row_index WHERE index_word IN (".implode(',',$words).")","index_word");
	//$todel=Array();
	//if(!empty($words)) foreach($words AS $w) if(empty($words2[$w])) $todel[$w]=$w;
	//if(!empty($todel)) $db->query("DELETE FROM index_word WHERE word_id IN (".implode(',',$todel).")",3,"index_word");*/
	$db->query("DELETE t1.* FROM index_word AS t1 LEFT JOIN row_index AS t2 ON t2.index_word=t1.word_id WHERE t2.index_word IS NULL");
}

function del_row_index($row){
	global $db;
	if(is_array($row)){
		$ids=implode(',',$row);
		$words=getall3($db,"SELECT index_col, index_word FROM row_index WHERE index_row IN ($ids)","index_word");
		$db->query("DELETE FROM row_index WHERE index_row IN ($ids)",3,"row_index");

	} else {
		$words=getall3($db,"SELECT index_col, index_word FROM row_index WHERE index_row=$row","index_word");
		$db->query("DELETE FROM row_index WHERE index_row=$row",3,"row_index");
	}
	if(!empty($words)) $words2=getall3($db,"SELECT index_word FROM row_index WHERE index_word IN (".implode(',',$words).")","index_word");
	$todel=Array();
	if(!empty($words)) foreach($words AS $w) if(empty($words2[$w])) $todel[$w]=$w;
	if(!empty($todel)) $db->query("DELETE FROM index_word WHERE word_id IN (".implode(',',$todel).")",3,"index_word");
}

function add_row_index($row,$as_job=0){
	global $db;
	$table=getrowval("SELECT * FROM main_row WHERE row_id=$row","row_table");
	if(empty($table)) return;
	$cols=getall3($db,"SELECT * FROM main_col WHERE col_table=$table AND col_index=1","col_id");
	if($as_job){
		foreach($cols AS $col) $db->query("INSERT INTO row_index (index_word, index_row, index_col) VALUES (0, $row, $col)",3,"row_index");	
	} else {	
		if(!empty($cols)) $vals=getall($db,"SELECT * FROM row_value WHERE value_col IN (".implode(',',$cols).") AND value_row=$row AND value_value!=''",1,"row_value");
		if(!empty($vals)) foreach($vals AS $val){	
			$rs=getall($db,"SELECT ro_ex, owner_id, row_id FROM row_owner WHERE row_id=$row",1,"row_owner");
			$rws=Array();
			if(!empty($rs)) foreach($rs AS $r){
				$rws[$r["row_id"]][]=$r;
			}
			$col=$val["value_col"];
			$val=$val["value_value"];
			$words=text2index($val,1);
			if(!empty($words)) foreach($words AS $word){
				if(!empty($rws[$row])) foreach($rws[$row] AS $cr){
					/*$db->query("SET NAMES `utf8` COLLATE `utf8_general_ci`");
					echo getrowval("SELECT * FROM index_word WHERE word_id=$word->lemma","word_lemma").' - ';
					echo $word->pos.'<br>';*/
					$db->query("INSERT INTO row_index (index_word, index_pos, index_row, index_col, index_tex, index_owner)
							VALUES (".$word->lemma.", ".$word->pos.", $row, $col, ".$cr["ro_ex"].", ".$cr["owner_id"].")",3,"row_index");
				}
			}
		}				
	}
}

function rebuild_row_index($row,$col=0){
	if($col!=0){
		$x=getrowval("SELECT col_id, col_index FROM main_col WHERE col_id=".$col,"col_index");
		if(!$x) return;
	} else {
		global $rlink,$table_index_cache;
		seek_rlink($row);
		if(!isset($table_index_cache[$rlink[$row]->table])) $table_index_cache[$rlink[$row]->table]=getrowval("SELECT col_id FROM main_col WHERE col_table=".$rlink[$row]->table." AND col_index=1","col_id");
		if(empty($table_index_cache[$rlink[$row]->table])) return false;
	}
	del_row_index($row);
	add_row_index($row);
}

function index_search($rows,$query,$cols="",$level="regular"){//easy regular strict
	//преобразуем многоуровневый массив к одному уровню
	global $rlink,$db;
	stf('index_search');
	$res=Array();
	$rows=collect_rows3($rows);
	$rr=Array();
	$rex=Array();
	$rown=Array();
	$table=0;
	if(!empty($rows)) foreach($rows AS $row) {
		if(is_object($row)) $id=$row->id;
		else $id=$row;
		seek_rlink($id);
		if(empty($rlink[$id])) continue;
		$r=$rlink[$id];			
		$table=$rlink[$id]->table;
		$rr[$id]=$id;
		$rex[$rlink[$id]->tex]=$rlink[$id]->tex;
		$rown[$rlink[$id]->owner]=$rlink[$id]->owner;
	}
	if(empty($cols)){
		$cols=getall3($db,"SELECT col_table, col_id, col_index FROM main_col WHERE col_table=$table AND col_index=1","col_id");
	} else {
		$cols=explode(',',/*parse_var(*/$cols/*)*/);
		$cols=implode("','",$cols);
		$cols=getall3($db,"SELECT col_table, col_id, col_index, col_sname FROM main_col WHERE col_table=$table AND col_sname IN ('$cols') AND col_index=1","col_id");
	}
	if(empty($cols)){ etf('index_search');	return $res;}
	/*$cols2=Array(); $i=0;
	foreach($cols AS $col){ $cols2[$col["col_id"]]=$i; $i++;}*/
	$words=text2index($query);
	if(empty($words)){ etf('index_search'); return $res;}
	$ws=Array();
	$ws2=Array();
	$i=0;
	foreach($words AS $word) if($word->lemma!=0){
		$ws[$word->lemma]=$word->lemma;
		$ws2[$i]=$word->lemma;
		$i++;
	}
	if(empty($ws)){etf('index_search'); return $res;}
	$tmp=getall($db,"SELECT * FROM row_index WHERE index_word IN (".implode(',',$ws).") AND index_tex IN (".implode(',',$rex).") AND index_owner IN (".implode(',',$rown).") AND index_col IN (".implode(',',$cols).")",1,"row_index");
	if(empty($tmp)){etf('index_search'); return $res;}
	$index=Array();
	$index2=Array();
	foreach($tmp AS $tm)if(!empty($rr[$tm["index_row"]])){
		//if($tm["index_row"]==35787) echo $tm["index_word"].' - '.$tm["index_pos"].'<br>';
		$index[$tm["index_row"]][$tm["index_word"]][$tm["index_col"]][$tm["index_pos"]]=1;
		$index2[$tm["index_row"]][$tm["index_col"]][$tm["index_word"]][$tm["index_pos"]]=1;
	}
	$matches=Array();//matches[score]=row_id
	if($level=="strict") $percent=100;
	if($level=="regular") $percent=70;
	if($level=="easy") $percent=50;			
	$may_pass=round(count($ws)/100*$percent+0.1);
	foreach($index AS $row=>$words){
		if(count($ws)==1 && !empty($words[$ws2[0]])) $matches[0][]=$row;
		if(count($ws)>1){
			$count=0;
			foreach($ws2 AS $i=>$lemma) if(!empty($words[$lemma])) $count++;
			if($count<$may_pass) continue; //YOU SHALL NOT PASS :)
			if($level=="strict") $max_distance=5;
			if($level=="regular") $max_distance=15;//30;
			if($level=="easy"){
				$matches[seek_nearness($index2[$row],0,0,count($ws),$row)][]=$row;
				continue;
			}
			$nearness=seek_nearness($index2[$row],$may_pass,$max_distance,count($ws),$row);
			//echo $nearness.'<br>';
			if($nearness>0) $matches[$nearness][]=$row;
		}
	}
	ksort($matches);
	if(!empty($matches)) foreach($matches AS $nearness=>$arr) foreach($arr AS $row) $res[]=$rows[$row];
	etf('index_search');
	return $res;
}

function seek_nearness($cols,$min_lems=0,$max_range=0,$max_lems=0,$row=0){//row - just for testing
	$min_nearest=0;
	$uses=Array();
	$repeat=0;
	foreach($cols AS $col=>$words){
		$text=Array();
		foreach($words AS $lemma=>$poses){
			if(!empty($uses[$lemma])) $repeat++;
			$uses[$lemma]=1;
			foreach($poses AS $pos=>$tmp) $text[$pos]=$lemma;
		}		
		$success=Array();		
		$lpos=-1;
		ksort($text);
		$first=true;
		$old_lemma=0;
		foreach($text AS $pos=>$lemma){
			if(!$lpos!=-1){
				$size=$pos-$lpos;
				//if($row==35787) echo $pos.'<br>';
				if($size<0) $size=-$size;
				if($max_range==0 || $max_range>=$size){
					if(!isset($success[$lemma]) || $success[$lemma]>$size) $success[$lemma]=$size;
					if($first){
						$success[$old_lemma]=0;
					}
					$first=false;
				}
			}
			$lpos=$pos;
			$old_lemma=$lemma;
		}
		if($min_lems==0 || count($success)>=$min_lems){
			$nearest=0;
			foreach($success AS $lemma=>$near) $nearest+=$near;
			if($max_lems!=0 && count($success)<$max_lems) $nearest+=($max_lems-count($success))*10;//max_lems - кол-во слов в запросе
			if($min_nearest==0 || $min_nearest>$nearest) $min_nearest=$nearest;
		}
	}
	if($min_nearest>0){
		$min_nearest=$min_nearest-$repeat;
		if($min_nearest<1) $min_nearest=1;
	}
	return $min_nearest;
}

function add_span_to_text($query,$text,$class='',$style='',$step_word=5,$max_word=80,$auto_index=1){
	$old_text=$text;
	$text=text2index($text,$auto_index);
	$query=text2index($query);
	if(empty($query) || empty($text)) return $old_text;
	$query2=Array();
	foreach($query AS $q) if(!empty($q->lemma)) $query2[$q->lemma]=1;
	if(empty($query2)) return $old_text;
	$lword=-1;
	$twords=0;
	$result='';
	if($step_word==0){					//highlight всего текста
		for($i=0;$i<count($text);$i++){
			if(!empty($query2[$text[$i]->lemma])){
				$result.='<span';
				if(!empty($class)) $result.=' class="'.$class.'"';
				if(!empty($style)) $result.=' style="'.$style.'"';
				$result.='>'.$text[$i]->old.'</span> ';
			} else $result.=$text[$i]->old.' ';
		}
	} else {							//highlight найденных областей
		for($i=0;$i<count($text);$i++){
			if(!empty($query2[$text[$i]->lemma]) && $i>$lword && ($twords<=$max_word || $max_word==0)){
				$st='';
				if($lword>$i-$step_word) $start=$lword+1; else $start=$i-$step_word;
				$end=$i+$step_word;
				if($end>=count($text)) $end=count($text)-1;
				if(!empty($result)) $result=trim($result).'… ';
				for($i2=$start;$i2<=$end;$i2++){
					if(!empty($text[$i2]->lemma) && !empty($query2[$text[$i2]->lemma])){
						$result.='<span';
						if(!empty($class)) $result.=' class="'.$class.'"';
						if(!empty($style)) $result.=' style="'.$style.'"';
						$result.='>'.$text[$i2]->old.'</span> ';
					} else if(!empty($text[$i2])) $result.=$text[$i2]->old.' ';
					$twords++;
					if($twords==$max_word) break(2);					
				}
				$lword=$i+$step_word;
			}
		}
	}
	return trim($result);
}

?>