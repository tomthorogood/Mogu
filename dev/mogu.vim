" Vim syntax file
" Language: MoguScript
" Maintainer: Tom A. Thorogood
" Latest Revision: 15 September 2012

if exists("b:current_syntax")
    finish
endif

let b:current_syntax="mogu"

syn keyword dictEntries type name styles signal on off message contained 
syn keyword dictEntries trigger listener listeners action block contained 
syn keyword dictEntries location source tooltip content degradation contained
syn keyword dictEntries strength animation auto contained

syn keyword synWords remove change test key next storage of
syn keyword synWords animate link do

syn keyword subWords style child value contained 
syn keyword subWords index session text contained 
syn keyword subWords enter action enter contained 
syn keyword subWords list string hash contained 
syn keyword subWords none node dynamic integer contained 
syn keyword subWords float file enum start contained 
syn keyword subWords left right bottom top contained 
syn keyword subWords pop fade text image events contained 

syn keyword wrappedValues reload clear this children siblings parent app
syn keyword wrappedValues bubble trickle rebroadcast store register slot
syn keyword wrappedValues match TERMINATE click mouseover mouseout
syn keyword wrappedValues newlook fail succeed keyup append replace
syn keyword wrappedValues regex text image submit radio textarea field
syn keyword wrappedValues dropdown checkbox multi container password stack

syn keyword dictNames events widgets perspectives global_events policies tree
syn keyword dictNames meta sessions validators

syn region pyDict start="{" end="}" fold transparent contains=dictEntries
syn region pyList start="[" end="]" fold transparent contains=pyDict
syn region pyStr start='"' end='"'
syn region pyStr start="'" end="'"

syn region sentence start="." end="\t" contains=subWords
syn region sentence start="." end=" " contains=subWords
syn region sentence start="." end="," contains=subWords
syn region sentence start="." end="\n" contains=subWords

hi def link dictEntries Statement
hi def link synWords Statement
hi def link subWords Statement
hi def link wrappedValues Statement
hi def link dictNames Type
hi def link pyStr Constant
hi def link sentence Statement
