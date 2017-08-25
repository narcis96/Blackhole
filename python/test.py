#thinker python
import json
def ConvertToIndex(line, pos)-> int:
    return int(line*(line+1)/2) + col

def ConvertToCell(indx) -> str:
    indx = indx + 1
    line = 0
    count = 0

    while True:
        if count + line + 1 >= indx:
            i = line
            j = indx - count - 1
            break
        line += 1
        count += line
    return chr(i + ord('A') - j) + chr(j + ord('1'))


if __name__ == '__main__':
    graph = dict()
    dx = [ 0, 0, -1, 1, -1, 1 ];
    dy = [ 1, -1, 0, 0, -1, 1 ];
    for i in range(8):
        for j in range(i+1):
            for k in range(6):
                line = i + dx[k]
                col = j + dy[k]
                if 0 <= line and line < 8 and 0 <=col and col <= line:
                    x = ConvertToIndex(i,j)
                    y = ConvertToIndex(line,col)

                    if x not in graph:
                        graph[x] = []
                    if y not in graph:
                        graph[y] = []
                    if y not in graph[x]:
                        graph[x].append(y)
                    if x not in graph[y]:
                        graph[y].append(x)
    for key in graph:   
        print(graph[key])