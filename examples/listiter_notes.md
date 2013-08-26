## Options for "Source" key:

+ group member

Represents each user who is a member of the active group

+ data

Represents a data node

+ group

Represents a group field

## Graphical Representation of Dynamic Table 

    { [c0^0] [c1^0] ... [cn^0] }
    { [c0^1] [c1^1] ... [cn^1] }
    { [c0^x] [c1^x] ... [cn^x] }

    {} = container of anonymous widgets
    sort will be on 'x'


## Algorithm 

    if widget type is container|stack and has source then:
✓       load data sources into vector
✓       load child list into vector
✓       initialize array of size x of Keyed_Assemblies

        for each source item (x):
            initialize string Key

            for each child (n):
                create unique name for anonymous widget
                based on source type, rewrite 'content' resolution for parsing
                set assembly content attribute as resolved content resolution
                load template data into assembly
                cache name and assembly
                if child iteration == sort index:
                    set Key to resolved assembly content
            
            create unique name for anonymous container widget
            set Keyed_Assembly[x]->assembly->name to unique name
            set Keyed_Assembly[x] Key to Key
            set Keyed_Assembly[x] attributes from database

        sort Keyed_Assembly array on array[x]->key

        for x in Keyed_Assembly array:
            append array->assembly->name to container children vector

## Example Code

    vector <string> src = srcnode.read();
    vector <string> children = chnode.read();
    vector <Keyed_Assembly<string>> assemblies {};
    Keyed_Assembly[src.size()] assemblies; 
    fill_n(assemblies, src.size(), Keyed_Assembly());

    for (int i = 0; i < src.size(); ++i)
    {
        std::string key {};
        int sort_index {get_sort_index(node)};
        
        Widget_Assembly* x {new Widget_Assembly{}};
        x->name = create_unique_name();
        unpack_data(x->attrdict);
        
        for (int c = 0; c < children.size(); ++c)
        {
            Widget_Assembly* a {new Widget_Assembly {}};
            a->name = create_unique_name();
            std::string content_tmpl {children[c]};
            std::string tmpl_name {get_template_name(content_tmpl)};
            a->attrdict[Mogu_Syntax::template_.integer] = tmpl_name;
            a->attrdict[Mogu_Syntax::text.integer] =
                resolve_value(content_tmpl,src[i]);
            unpack_data(a->attrdict,tmplname)
            cache_add(a);
            if (c==sort_index)
                key = a->attrdict[Mogu_Syntax::text.integer];
            x->children.push_back(a->name);
        }
       
        assemblies[i].set_key(key);
        assemblies[i].set_assembly(x); 
        cache_add(x);
    }

    sort(assemblies);
    for (Keyed_Assembly x : assemblies)
    {
        container->children.push_back(x->name);
    }
