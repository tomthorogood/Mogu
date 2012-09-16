" Vim syntax file
" Language: MoguScript
" Maintainer: Tom A. Thorogood
" Latest Revision: 15 September 2012

if exists("b:current_syntax")
    finish
endif


syn keyword dictEntries type name styles signal message contained 
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
syn keyword wrappedvalues on off

syn keyword dictNames events widgets perspectives global_events policies tree
syn keyword dictNames meta sessions validators

syn match digits '\d\+' contained

syn match pyStr /\".*\"/ contained
syn match pyStr "'.*'" contained
syn match numFn 'Number(\d\+)' contained contains=digits

syn match pyFlow '\s.*\.' contained nextgroup=subWord
syn region pyDict transparent start="{" end="}" contains=dictEntries,subWords,wrappedValues,pyStr,pyFlow,numFn
syn region pyList transparent start="\[" end="]" contains=pyStr,pyDict

hi def link dictEntries Statement
hi def link synWords Special
hi def link subWords Statement
hi def link wrappedValues Constant
hi def link dictNames Type
hi def link pyStr Constant
hi def link pyDict Keyword
hi def link pyList Function
hi def link pyFlow Function
hi def link numFn Function
hi def link digits Constant

let b:current_syntax="mogu"
