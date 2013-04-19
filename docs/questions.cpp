/* First, we need to explore exactly how the NVP will return input.
 * This may seem obvious, but given the changes to everything, I want
 * to be assured of some things before continuing to rewrite the rest
 * of the engine that needs to change.
 */ 

// Get a widget's type from the database:
Redis::ContextQuery context(Prefix:widgets);
context.appendQuery(
    std::make_shared <Redis::Query> ( 
        new Redis::Query(
            "hget widgets.%s %d", widget_identifier, (int) MoguSyntax::type
        )));

NodeValue v;
const char* response = context.yieldResponse <const char*>();
nvp.give_input(response,&v);

MoguSyntax widget_type = (MoguSyntax) v.getInt();

switch (widget_type) {
    case MoguSyntax::text: 
        //create text widget
        break;
    case MoguSyntax::image:
        //create image widget
        break;
    //and so forth
}

/* None of the above should be any sort of surprise, but I wanted to verify
 * with actual code how everything we've done will tie together.
 *
 * Now, something a little more complicated: 
 * Say to keep the stylings DRY, we have styling stored
 * in a hash node 'styles', whose fields represent the type
 * of style to be applied:
 *      'happy_style'    :   'someCSS_class w10 foo bar'
 *      'sad_style'      :   'someCSS_class baz blip'
 *
 * The MoguScript entry would be:
 *
 *  css     :   data style happy_style
 *
 *  This would be stored in the database as something like:
 * <field>  : <value>
 *  30      :   40 styles happy_style
 *
 * I'll use the same ContextQuery and NodeValue objects as before.
 */

context.appendQuery(
    std::make_shared <Redis::Query> (
        new Redis::Query(
            "hget widgets.%s %d", widget_identifier, (int) MoguSyntax::css
        )));

response = context.yieldResponse <const char*>();
nvp.give_input(response,&v);

std::string style = v.getString();
// 'style' should now hold "someCSS_class w10 foo bar"

/* Again, no surprises when it comes to the resolution of attributes,
 * but what are we going to do about events?
 * Before, when events were horrible and stored as hash nodes, it took a lot of
 * work and a few different data types to represent them. Now that they are being
 * stored as command strings, how are we handling them?
 *
 * For instance:
 *  when mouseover {
 *      set own style to data styles sad_stlye
 *  }
 *  
 *  The command itself (set own style...) will be stored
 *  in a list attributed to the 'mouseover' trigger (this part is no problem),
 *  and the command translates to: 
 *      '25 1 30 59 40 styles sad_style'
 *
 *  ...what exactly are we going to do with that? Is this somethign I should be
 *  working on, or is it something you've already taken into consideration?
 
 *  If that string gets passed into the NVP, how will the NVP return the input?
 *  The result certainly falls out of the scope of a NodeValue object:
 *
 *  '25'                    -> action <int>
 *  '1'                     -> pointer to calling widget <Moldable*>
 *  '30'                    -> attribute <int>
 *  '59'                    -> prepositional delimiter (not stored or used in processing) 
 *  '40 styles sad_style'   -> "comsCSS_class baz blip" <string|const char*>
 *
 * I know you mentioned a while back that you were removing the event 
 * preprocessor, etc. to make all of that more efficient, and I'm all fo rit, 
 * but I need to know:
 *  1) Are you working on building the new handler?
 *      Is the CommandParser doing this?
 *  2) If so, how are you implementing it?
 *  3) If not, and it's something I'm supposed to be working on,
 *      what return data type will the NVP give me to work with?
 */


