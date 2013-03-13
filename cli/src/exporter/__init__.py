import MoguExporter
import redis
import re
import sys
import os

prefix_index = {
    "widgets"       :   MoguExporter.Widget,
    "templates"     :   MoguExporter.Template,
    "policies"      :   MoguExporter.Policy,
    "validators"    :   MoguExporter.Validator,
    "data"          :   MoguExporter.Data
        }

def is_root(string):
    return len(re.findall("\.",string)) == 1

def prefix(string):
    return string.split(".")[0].strip()

def export(args):
    db = redis.Redis(
            db = args.redis_db,
            host=args.redis_host,
            port=args.redis_port
            )
    roots = filter(is_root,db.keys("*"))
    if args.output:
        os.system("touch %s" % args.output)
    for root in roots:
        cls = prefix_index[prefix(root)]
        obj = cls(root,db)
        if not args.output:
            try:
                sys.stderr.write(obj.export())
            except redis.exceptions.ResponseError as e:
                sys.stderr.write("Cannot export root: %s" % root)
                raise e
        else:
            with open(args.output,"a") as f:
                f.write(obj.export())

if __name__ == "__main__":
    import redis
    db = redis.Redis()
    export(db)
