class DictStorage:
    is_merge            =0x1 # Existing data will be overwritten, but archived data will be preserved (if off, node will be deleted first)
    assume_yes          =0x2 # If off, "assume_yes" will be implied
    is_test             =0x4

class ListStorage:
    is_merge            =0x1
    assume_yes          =0x2
    merge_no_repeat     =0x4 # If no_repeat is on, the merged list will remove pre-existing entries in the database in favor of new data
    is_test             =0x8



def is_set(byte,bit):
    return bool(byte & bit)

def set_bit(byte,bit):
    return byte | bit
