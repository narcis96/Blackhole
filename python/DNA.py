class DNA(object):
    def __init__(self, name, arhitecture):
        self.__name = name
        for i in range(len(arhitecture)):
            hidden_layer = [{'weights':[random() for j in range(arhitecture[i] + 1)]}]
            self.__network.append(hidden_layer)
        # output_layer = [{'weights':[random() for i in range(n_hidden + 1)]} for i in range(n_outputs)]
        # network.append(output_layer)
    def CrossOver(other, name):





