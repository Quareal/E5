[glob.form_id=glob.form_id+++1][form_id=glob.form_id++glob.rand(0,1000)]
[$pre_html]
[if !$only_body]
	[if $anchor]<a name="[$anchor]"></a>[/if]	
	<h2[if $hidden] OnClick="showhide('form[$form_id]')" style="cursor: pointer;"[/if]>[if $icon][icon($icon)] [/if][$name]</h2>
	[if $hidden]
		<div id="form[$form_id]" style="display: none;">
	[/if]
	<form action="[$path]" method="post" enctype="multipart/form-data"[if $OnSubmit] OnSubmit="[$OnSubmit]"[/if]>
[/if]
[block name="element" silent]
	[glob.e_count=glob.e_count+++1]
	[tree $fields]
		[t=value['type']]
		[s=value['sub']]
		[if $t!='hidden']<div style="padding-bottom: 5px;">[/if]
		[if value['title']][value['title']]:<br>[/if]
		[if $t=='static']
			[value['content']]
		[else]
			[tree value][if var!='title'][param.{^var}=value][/if][/tree]
			[include.editor.inputs.{^$t}]
			[tree value][if var!='title'][param.{^var}=''][/if][/tree]
		[/if]
		[if $s]
			<div id="fold[glob.e_count]" style="[if !value['value']]display: none; [/if]padding: 20px; background-color: #FAFAFA;">
				[block.element(fields=$s)]
			</div>
		[/if]
		[if $t!='hidden']</div>[/if]
	[/tree]
[/block]
[tree $section]
	[if value['title']]
		<h2 [if value['hidden']]OnClick="showhide('section[$form_id]_[index]');"[/if] style="cursor: pointer;">[if value['icon']][icon(value['icon'])][/if][value['title']]</h2>
		[add='none']
		[tree value['fields']][if value['value']][add=''][break][/if][/tree]
		<div id="section[$form_id]_[index]" style="display: [$add];">
	[/if]
	[block.element(fields=value['fields'])]
	[if value['title']]
		</div>
	[/if]
[/tree]
[if !$only_body]
	[if $form_type=='edit']<input class="button" type="submit" value="[lng('Save')]"> [lng('or')] <a href="[if $go_back_url][$go_back_url][else][$path][/if]">[lng('go back')]</a>[/if]
	[if $form_type=='add']<input class="button" type="submit" value="[if $btn_title][$btn_title][else][lng('Add')][/if]">[/if]
	</form>
[/if]
[if $hidden]
	</div>
[/if]