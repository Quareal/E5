<?php

	clean_magic_quotes_once($query);
	echo '<form action="mod_main" method="post" style="margin: 0px; padding: 0px;">';
	echo '<br><table cellpadding="0" cellspacing="0" width="100%"><tr><td valign="center" align="left"><h2>Результаты поиска</h2></td>
	<td align="right" valign="center" width="310">
			<input type="hidden" name="id" value="'.$id.'">
			<input type="hidden" name="action" value="table_search">
			'.si('search').'
			<input type="text" name="query" value="'.htmlspecialchars($query).'" style="width: 250px;">
			<input type="submit" value=">" class="arrow_btn">
	</td></tr></table>';
	echo '<div align="right" style="margin-top: -5px;">';
	echo '<label style="cursor: pointer;">только по текущему разделу <input type="checkbox" name="search_ex"'.(!empty($search_ex)?'checked':'').' class="button"></label>';
	echo '</div>';
	echo '</form>';
	if(empty($search_ex)) $search_ex=false;

	$search=safe_sql_input($query);
	
	if(empty($limit)) $limit=30;
	if(empty($page)) $page=1;
	$start=($page-1)*$limit;
	$end=$start+$limit;
	

	if(isset($_COOKIE["cex".$id])) $eid=$_COOKIE["cex".$id];	
	else $eid=0;
	
	$exs='';
	if(!empty($search_ex) && !empty($eid)){
		$all_ex=getall3($db,"SELECT * FROM ex_group WHERE ex_ex2=$eid","ex_ex1");
		if(!empty($all_ex)) $exs.=" AND o.ro_ex IN (".implode(',',$all_ex).")";
	}

	$res=Array();$cres=0;
	$r_tables=Array();
	$r_cols=Array();
	$b_cols=Array();
	$r_exes=Array();
	$a_exes=Array();
	$rcount=getrowval("SELECT count(*) AS cnt FROM row_value AS v LEFT JOIN row_owner AS o ON o.row_id=v.value_row WHERE v.value_module=$id ".$exs." AND v.value_value LIKE '%$search%' AND v.value_table!=0 GROUP BY v.value_row","cnt");
	$rows=getall($db,"SELECT v.value_value, v.value_row, v.value_module, v.value_table, v.value_col, o.ro_ex FROM row_value AS v LEFT JOIN row_owner AS o ON o.row_id=v.value_row WHERE v.value_module=$id ".$exs." AND v.value_value LIKE '%$search%' AND v.value_table!=0 GROUP BY v.value_row LIMIT $start, $limit");
	if(!empty($rows)) foreach($rows AS $row){
		$res[$cres]->row=$row["value_row"];
		$res[$cres]->col=$row["value_col"];
		$res[$cres]->ex=$row["ro_ex"];
		$res[$cres]->table=$row["value_table"];
		//$res[$cres]->value=as_html($row["value_value"]);
		$r_cols[$row["value_col"]]=$row["value_col"];
		$r_tables[$row["value_table"]]=$row["value_table"];
		if(!empty($row["ro_ex"])) $r_exes[$row["ro_ex"]]=$row["ro_ex"];
		$cres++;
	}
	if(!empty($r_tables)) $a_tables=getall6($db,"SELECT table_id, table_name FROM main_table WHERE table_id IN (".implode(',',$r_tables).")","table_id","table_name");
	if(!empty($r_cols)) $a_cols=getall6($db,"SELECT col_id, col_name FROM main_col WHERE col_id IN (".implode(',',$r_cols).")","col_id","col_name");
	if(!empty($r_exes)){
		$a_exes2=getall6($db,"SELECT * FROM ex_group WHERE ex_ex1 IN (".implode(',',$r_exes).")","ex_ex1","ex_ex2");
		if(!empty($a_exes2)){
			if(!empty($eid) && !empty($search_ex)){
				foreach($a_exes2 AS $var=>$value) $a_exes2[$var]=$eid;
			}
			$a_exes3=getall6($db,"SELECT ex_id, ex_name FROM ex_module WHERE ex_id IN (".implode(',',$a_exes2).")","ex_id","ex_name");
			if(!empty($a_exes3)) foreach($a_exes2 AS $ex1_id=>$ex2_id) if(!empty($a_exes3[$ex2_id])) $a_exes[$ex1_id]=$a_exes3[$ex2_id];
		}
	}
	if(!empty($r_tables)){
		$b_cols=getall6($db,"SELECT col_id, col_url, col_table FROM main_col WHERE col_table IN (".implode(',',$r_tables).") AND col_url=1","col_table","col_url");
	}
	if(!empty($res)){
		// Пагинатор	
		$crows=$rcount;
		$start=0;$end=$crows-1;
		if($crows>30){
			$href="mod_main?id=$id&amp;search_ex=$search_ex&amp;limit=$limit&amp;action=table_search&amp;query=$query";
			$href.='&amp;page=';
			$max=$crows;
			echo '<table cellpadding="0" cellspacing="0" style="margin-bottom: 4px;"><tr><td>';
			echo '<div style="margin-right: 15px; margin-left: 5px;">';
			if($max>$limit){
				echo '<div class="pagebar">';
				$cp=floor($max/$limit);
				if($max%$limit!=0) $cp++;
				if($page>$cp) $page=$cp;
				$num=12;
				$p_start=1;
				$p_end=$cp+1;
				if($page>$num/2+1){
					$p_start=$page-$num/2+1;
					echo '<div class="page-box link" style="margin-right: 30px;" OnClick="document.location.href=\''.$href.(1).'\';" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.(1).'" class="trueblue">1</a></div>';
				}
				if($page<$cp-$num/2-1){
					$p_end=$page+$num/2+1;
				}
				if($p_start>$cp) $p_start=$cp;
				if($p_end<1) $p_end=1;
				if($p_start<1) $p_start=1;
				if($p_start>$p_end) $p_start=$p_end;
				for($i=$p_start-1;$i<$p_end-1;$i++){
					if($i+1==$page) echo '<div class="page-box pb-active">'; else echo '<div class="page-box link" OnClick="document.location.href=\''.$href.($i+1).'\';" OnMouseOver="this.style.backgroundColor=\'#1076DC\'; this.firstChild.style.color=\'#FFFFFF\';"  OnMouseOut="this.style.backgroundColor=\'#FFFFFF\'; this.firstChild.style.color=\'#1076DC\';"><a href="'.$href.($i+1).'" class="trueblue">';
					echo ($i+1);
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
			echo '<form name="tableform2" action="mod_main" method="post" style="margin: 0px; padding: 0px;">
			<input type="hidden" name="id" value="'.$id.'">
			<input type="hidden" name="search_ex" value="'.$search_ex.'">
			<input type="hidden" name="action" value="table_search">
			<input type="hidden" name="query" value="'.$search.'">';
			echo '<input type="text" name="limit" value="'.$limit.'" class="mini_input"><input type="submit" class="arrow_btn" value=">">';
			echo '</form>';
			echo '</td></tr></table>';
		}	
		// Вывод результатов
		echo '<table id="records" cellpadding="3" cellspacing="1"><tr><th>Заголовок</th><th>Экземпляр</th><th>Таблица</th><th>Поле</th><th width="105">Действие</th></tr>';
		foreach($res AS $r){
			seek_rlink($r->row);
			if(check_row($r->row,$r->table,get_ex2($rlink[$r->row]->tex),'view',$rlink[$r->row]->user,$rlink[$r->row]->users,$id)){
				echo '<tr>';
				echo '<td>'.get_basename($r->row,$r->table).'</td>';
				if(!empty($a_exes[$r->ex])){
					echo '<td>'.$a_exes[$r->ex].'</td>';
				} else echo '<td>-</td>';
				echo '<td>'.$a_tables[$r->table].'</td>';
				echo '<td>'.$a_cols[$r->col].'</td>';
				echo '<td>';
				if(check_row($r->row,$r->table,get_ex2($rlink[$r->row]->tex),'edit',$rlink[$r->row]->user,$rlink[$r->row]->users,$id)){
					echo se('edit',get_admin_url($r->row));
				} else {
					echo si('protect');
				}
				if(!empty($b_cols[$r->table])){
					$url=get_row_url($r->row);
					if(!empty($url)) echo se('anchor',$url);
				}
				echo '</td>';
				echo '</tr>';
			} else {
				echo '<tr>';
				echo '<td>'.si('protect').'</td>';
				echo '<td>'.si('protect').'</td>';
				echo '<td>'.si('protect').'</td>';
				echo '<td>'.si('protect').'</td>';
				echo '</tr>';
			}
		}
		echo '</table>';
		
	} else echo '<br><div><b>Результатов не найдено</b></div>';
	echo '<br><div><a href="mod_main?id='.$id.'">'.si('back').'Вернуться к списку таблиц и разделов</a></div>';

?>