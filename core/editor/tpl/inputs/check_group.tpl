[step=0]
[block name="checkbox_group"]
	[tree $items]
		<div style="margin-left: 5px;">
			<label style="cursor: pointer;">
				[if $grand_name]
					[cname=$grand_name+'['+value['id']+']']
				[else]
					[cname=value['id']]
				[/if]
				<input type="checkbox" name="[$cname]" id="[$cname]" class="button" [value['addon']] [if $value[value['id']]] checked[/if]/> [value['value']]
			</label>
			[if value['sub']][block.checkbox_group(items=value['sub'])][/if]
		</div>
	[/tree]
[/block]