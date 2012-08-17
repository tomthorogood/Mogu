from db_type import *
from redis import Redis
from db_type_exceptions import *

db = Redis()

fdef = {
        "type"  :   "text"
        }

fcdef = ["Foo", "bar", "baz"]

f = Widget()
f.build("test_widget")
f.exists(db)
try:
    f._import(db, fdef, 0)
except AttributeNotFoundError:
    fdef["content"] = "hello!"
    f._import(db, fdef,0)

fc = WidgetChildren()
fc.build("test_widget")
print fc.exists(db)
fc._import(db, fcdef, 0)
print db.llen(fc.node)
db.delete(f.node)
db.delete(fc.node)

