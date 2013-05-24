# Factories/MoldableFactory.cpp #

MoldableContainer::init
    -> MoldableAbstractParent::init
        -> Moldable::init
MoldableContainer::load
    -> MoldableAbstractParent::load
        -> Moldable::load

MoldableStack::init
    ->  MoldableAbstractParent::init
    ->  WFriendlyStack()
    
