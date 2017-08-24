import json
class cls(object):
    def __init__(self):
        self.__value = 0
class Object:
    def toJSON(self):
        return json.dumps(self, default=lambda o: o.__dict__, 
            sort_keys=True, indent=4)


if __name__ == '__main__':
    obj = cls()
#    obj.x = 1
    print(obj.x)