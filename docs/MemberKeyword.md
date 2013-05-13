# The 'Member' Keyword #


## Introduction #

Mogu's current functionality is great for more static sites. Creating an 
application with nice effects, even adding users, is easy to do. Sadly, the
user/group functionality is still absent. It has taken  a lot of brainstorming
to figure out how to implement this while keeping the syntax for MoguScript
tight. 

Interacting with multiple data fields at the same time is an obvious
requirement. Pretty much any "group"-centric applications has moderators that
need to be able to, well, moderate. Doing that requires some functionality to 
view and edit their consituents' data. 

## Introducing the "Member" Keyword #

To solve this problem, we're implementing one keyword to rule them all: MEMBER.

Depending on its context, this keyword will mean multiple things in regards to 
how it functions, but should only mean one thing to you. Creating a dynamic
widget is a three-step process.

**First, you have to set permissions in a piece of data's storage policy.**

    policy personal_info
        type        :   hash
        read        :   user moderator
        write       :   user
        encrypted   :   yes
    end policy

The above example would allow a user and their moderator to view fields in 
their personal\_info hash, but only allow users to edit this information. 

For our example, let's pretend we want to create a table allowing moderators
to view the height of all their group's users. 

**We then need to create a container for this information**

Let's create a simple table of "username: height" lines. 

    widget group_heights
        type        :   container
        access      :   moderator 
        content     :   member personal_info
        template    :   height_row
    end widget

Above, you'll see we did something that seems to conflict: created a container
widget and assigned it content, which would seemingly have no effect. The 
trick here is that the content will be applied to the template, and the template
will have nothing to do with the widget 'group\_heights'. You'll also notice
that even though 'personal\_info' is a hash field, we did not assign it any
arguments. That's fine. The identifier alone is enough to look up the access
rights; the actual assignments come next:

**Next, create the view for the dynamic information**

    template height_row
        type        :   container
        children
            :username
            :height
        end children
    end template

    template height_row:username
        type        :   text
        content     :   member personal_info username
    end template

    template height_row:height
        type        :   text
        content     :   member personal_info height
    end template

Above, you can see that just as if we were creating a similar widget set with
static data, we've done so with templates. It should be pretty clear how 
this works from a high level perspective.

## Behind the Scenes #

### How will Mogu resolve this information?  #

When Mogu sees a template and content applied to a container, it first checks
the access rights of 'content' to make sure that the user instantiating the
widget has access to this content. 

Then, it looks up the list of all members of the group. For each of those
members, it will apply the specific data to anywhere it is needed within the
template set.

In theory, this could be expanded to have even more uses, such as showing
all elements in a list field for every user (for instance, showing a user a list
of all of their interactions within an application), and can also work for 
editing user data by applying actions. If above, the moderator had write access,
we could have also done something like this:

    template height_row:edit
        type    :   input
        content :   member personal_info height
        
        events
            when keyup {
                set member personal_info height to own text
            }
        end events
    end template

## Security Concerns #

Ownership of data is, of course, huge. Currently, this schematic offers users
very little control of what can be accessed by moderators, and that's bad. 
With a little event magic, however, this can be dealt with. If you want to give
users the option to hide this data from moderators, for instance, you could 
create an option that works as follows:

    policy private_personal_info
        type    :   hash
        read    :   user
        write   :   user
        encrypted:  yes
    end policy

    widget make_private
        type    :   text
        access  :   user
        css     :   "linkCursor" # make this appear as a link when hovered
        
        events
            when clicked {
                set user private_personal_info username to user personal_info username
                set personal_info username to "HIDDEN"
                set user private_personal_info to user personal_info height
                set personal_info height to "HIDDEN"
                set user personal_info_store to "private_personal_info"
            }
        end events
    end widget

To display this data to the user, you would then do something like this:

    widget my_height
        type    :   text
        access  :   user
        content :   user user personal_info_store height
    end widget

In the above example, "user personal\_info\_store" would be resolved first,
followed then by "user private\_personal\_info", which would actually retrieve
the height. The group moderator would thus only see 'HIDDEN', but the user 
would be able to access their own data.

Fun!


end blogpost
