<select name="[$name]" id="[$id]" [$addon]>
[step=0]
[block name="select_group"]
	[tree $items]
		<option value="[value['id']]"[value['addon']][if value['id']==$value] selected[/if]>[value['value']]</option>
		[if value['sub']]
			[block.select_group(items=value('sub'), step=$step+1)]
		[/if]
	[/tree]
[/block]
</select>