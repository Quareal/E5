[$pre_html][skip empty]
<table id="records" cellpadding="3" cellspacing="1">
[if $th]
	[* Заголовки таблицы *]
	<tr>
		[tree $th]
			<th>[value]</th>
		[/tree]
	</tr>	
	[cols_count=$th.count]
[else]
	[cols_count=$rows.first['cols'].count]
[/if]
[* Вывод строк *]
[tree $rows]
	<tr id="[value['id']]" style="[value['style']]" class="[value['class']]">
	[if value['subtable']]
		<td colspan="[$cols_count]">[include.editor.block.table(rows=value['subtable'], th=false, pre_html='', post_html='')]</td>
	[else]
		[tree value['cols']]
			<td>	[value]</td>
		[/tree]
	[/if]
	</tr>
[/tree]
</table>
[$post_html]