<?php
	clean_magic_quotes_once($query);
	echo '<br><table cellpadding="0" cellspacing="0" width="100%"><tr><td valign="center" align="left"><h2>Результаты поиска</h2></td>	
	<td align="right" valign="center" width="310">
		<form action="mod_main" method="post" style="margin: 0px; padding: 0px;">
			<input type="hidden" name="id" value="'.$id.'">
			<input type="hidden" name="action" value="part_search">
			'.si('search').'
			<input type="text" name="query" value="'.htmlspecialchars($query).'" style="width: 250px;">
			<input type="submit" value=">" class="arrow_btn">
		</form>
	</td></tr></table>';

	$search=safe_sql_input($query);
	
	$res=Array();
	
	function add_part_res(&$res, $search, $title, $data, $url){
		$res[]='<tr><td>'.$title.'</td><td>'.se('edit',$url).'</td></tr>';
	}
	
	$tmp=getall($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type IN (0,1,3,4,5) AND part_body LIKE '%".$search."%'");
	if(!empty($tmp)) foreach($tmp AS $part){
		add_part_res($res, $query, 'Часть "'.$part["part_name"].'"', $part["part_body"], 'mod_part?id='.$id.'&id2='.$part["part_id"]);		
	}
	$tmp=getall($db,"SELECT * FROM main_col WHERE col_module=$id AND (col_onform LIKE '%".$search."%' OR col_oninsert LIKE '%".$search."%' OR col_onshow LIKE '%".$search."%')");
	if(!empty($tmp)) foreach($tmp AS $col){
		$name='Переменная "'.$col["col_name"].'"';
		if($col["col_table"]!=0) $name.=' таблицы "'.getrowval("SELECT table_name, table_id FROM main_table WHERE table_id=".$col["col_table"],"table_name").'"';
		add_part_res($res, $query, $name, $col["col_onform"].$col["col_oninsert"].$col["col_onshow"], 'mod_col?id='.$id.'&id2='.$col["col_table"].'&id3='.$col["col_id"].'&action=edit_col_form#edit');
	}
	$tmp=getall($db,"SELECT * FROM main_table WHERE table_module=$id AND (table_onedit LIKE '%".$search."%' OR table_bottom LIKE '%".$search."%' OR table_top LIKE '%".$search."%')");
	if(!empty($tmp)) foreach($tmp AS $tbl){
		add_part_res($res, $query, 'Таблица "'.$tbl["table_name"].'"', $tbl["table_onedit"].$tbl["table_bottom"].$tbl["table_bottom"], 'mod_main?id='.$id.'&id2='.$tbl["table_id"].'&action=edit_table_form#edit_table');
	}
	$tmp=getall($db,"SELECT * FROM main_part WHERE part_module=$id AND (part_ifcase LIKE '%".$search."%' OR part_ifrow LIKE '%".$search."%')");
	if(!empty($tmp)) foreach($tmp AS $part){
		add_part_res($res, $query, 'Часть "'.$part["part_name"].'" (параметры)', $part["part_ifrow"].$part["part_ifcase"], 'mod_main?id='.$id.'&id2='.$part["part_id"].'&action=edit_part_form#edit_part');
	}
	
	if(empty($res)){
		echo '<br><div><b>Результатов не найдено</b></div>';
	} else {
		echo '<table id="records" cellpadding="3" cellspacing="1"><tr><th>Название</th><th width="105">Действия</th></tr>';
		foreach($res AS $r) echo $r;
		echo '</table>';
	}
	echo '<br><div><a href="mod_main?id='.$id.'">'.si('back').'Вернуться к списку частей</a></div>';
	
?>