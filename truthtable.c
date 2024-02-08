#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef enum {
    PASS,
    OR,
    NOT,
    NOR,        
    AND,        
    NAND,        
    MULTIPLEXER,
    DECODER,
    XOR,
    UNKNOWN_TYPE
} GateType;

typedef struct {int out; char *name; int boolean;} Vari;


char *gateName[9] = {
    "AND",
    "NOT",      
    "PASS"
    "XOR",          
    "OR",        
    "NAND",       
    "NOR",           
    "MULTIPLEXER",        
    "DECODER",      
};


typedef struct {GateType type; int *parameters; int size;} Gate;



typedef struct Circuit {
    int numOfGates, numOfOutputs, numOfInputs, *inputs, *outputs, numOfVariables;
    Gate **gateList;
    Vari **variableArray;
} Circuit;



void freeGate(Gate *gate) {
    if (!gate) return;

    free(gate->parameters);
    free(gate);
}


void freeCircuitInputs(Circuit *circuit) {
    if (circuit->numOfInputs) {
        if (circuit->inputs) {
            free(circuit->inputs);
        }
    }
}


void freeCircuitOutputs(Circuit *circuit) {
    if (circuit->numOfOutputs) {
        if (circuit->outputs) {
            free(circuit->outputs);
        }
    }
}



void freeCircuitGates(Circuit *circuit) {
    if (circuit->gateList == NULL) {
        return;
    }

    int i = 0;
    do {
        freeGate(circuit->gateList[i]);
        i++;
    } while (i < circuit->numOfGates);

    free(circuit->gateList);
}



void varFree(Vari *variable) {
    if (!variable)
        return;

    free(variable->name);
    free(variable);
}



void freeCircuitVariables(Circuit *circuit) {
    if (circuit->variableArray == NULL) {
        return;
    }

    int i = 0;
    do {
        varFree(circuit->variableArray[i]);
        i++;
    } while (i < circuit->numOfVariables);

    free(circuit->variableArray);
}



void freeCircuit(Circuit *circuit) {
    if (circuit != NULL) {
        freeCircuitInputs(circuit);
        freeCircuitOutputs(circuit);
        freeCircuitVariables(circuit);
        freeCircuitGates(circuit);

        free(circuit);
    }
}



int isValidGateInput(GateType type, int *parameters, int numParameters, Circuit *circuit) {
    
    switch (type) {
        case PASS:
        case NOT:
            return (numParameters == 2);
        
        case AND:
        case OR:
        case XOR:
        case NAND:
        case NOR:
            return (numParameters == 3);
        
        case DECODER:
     
            return (numParameters == circuit->gateList[circuit->numOfGates - 1]->size + (1 << circuit->gateList[circuit->numOfGates - 1]->size));
        
        case MULTIPLEXER:
            return (numParameters == circuit->gateList[circuit->numOfGates - 1]->size + (1 << circuit->gateList[circuit->numOfGates - 1]->size) + 1);
        
        default:
            return 0;
    }
}


void printGateDetails(Gate *gate, Circuit *circuit) {
    printf("Gate Type: %d\n", gate->type);
    printf("Number of Parameters: %d\n", gate->size);
    printf("Parameters: ");
    for (int i = 0; i < gate->size; ++i) {
        printf("%d ", gate->parameters[i]);
    }
    printf("\n");
}



void printCircuitDetails(Circuit *circuit) {
    printf("Number of Gates: %d\n", circuit->numOfGates);
    printf("Number of Inputs: %d\n", circuit->numOfInputs);
    printf("Inputs: ");
    for (int i = 0; i < circuit->numOfInputs; ++i) {
        printf("%d ", circuit->inputs[i]);
    }
    printf("\n");
    printf("Number of Outputs: %d\n", circuit->numOfOutputs);
    printf("Outputs: ");
    for (int i = 0; i < circuit->numOfOutputs; ++i) {
        printf("%d ", circuit->outputs[i]);
    }
    printf("\n");
}



int isInputGate(Gate *gate) {
    return (gate->type == PASS || gate->type == NOT);
}

int isOutputGate(Gate *gate) {
    return (gate->type == DECODER || gate->type == MULTIPLEXER);
}



int findVariableIndex(char *var, Circuit *circuit) {
    int index = -1;
    int i = 0;

    if (circuit->numOfVariables == 0) {
        return index;
    }

    while (i < circuit->numOfVariables) {
        if (!strcmp(var, circuit->variableArray[i]->name)) {
            index = i;
        }

        if (index == -1) {
            i++;
        } else {
            break; 
        }
    }

    return index;
}



const char *gateTypeToString(GateType type) {
    switch (type) {
        case PASS: return "PASS";
        case NOT: return "NOT";
        case AND: return "AND";
        case OR: return "OR";
        case XOR: return "XOR";
        case NAND: return "NAND";
        case NOR: return "NOR";
        case DECODER: return "DECODER";
        case MULTIPLEXER: return "MULTIPLEXER";
        default: return "UNKNOWN";
    }
}



Vari *createNewVariable(char *name, Circuit *circuit) {
    Vari *var;
    var = malloc(sizeof(*var));
    var->name = (char *) malloc(strlen(name) + 1);
    if (var->name == NULL) {
    free(var);
    return NULL;
}
    strcpy(var->name, name);
    var->out = 0;
    var->boolean = -1;
    return var;
}


int addVariable(char *varName, Circuit *circuit) {
    int index = findVariableIndex(varName, circuit);

    if (index != -1) {
        return index;
    }


    index = circuit->numOfVariables;

    Vari **variableArray = (Vari **)realloc(circuit->variableArray, (index + 1) * sizeof(Vari *));
    if (variableArray == NULL) {
        return -1; 
    }

    circuit->variableArray = variableArray;
    circuit->variableArray[index] = createNewVariable(varName, circuit);

    if (circuit->variableArray[index] == NULL) {
        return -1; 
    }

    circuit->numOfVariables++;
    return index;
}



Circuit *initializeCircuit() {
    Circuit *circuit = (Circuit *)malloc(sizeof(Circuit));

    if (circuit == NULL) {
        return NULL;
    }

    memset(circuit, 0, sizeof(Circuit));

    int index_0 = addVariable("0", circuit);
    int index_1 = addVariable("1", circuit);

    if (index_0 == -1 || index_1 == -1) {
        freeCircuit(circuit);
        return NULL;
    }

    circuit->variableArray[index_0]->boolean = 0;
    circuit->variableArray[index_1]->boolean = 1;

    if (addVariable("_", circuit) == -1) {
        freeCircuit(circuit);
        return NULL;
    }

    return circuit;
}



Gate *createNewGate(GateType type, int n, int size, Circuit *circuit) {
    Gate *gate = (Gate *)malloc(sizeof(Gate));

    if (gate == NULL) {
        return NULL;
    }

    gate->type = type;
    gate->parameters = (int *)malloc(n * sizeof(int));

    if (gate->parameters == NULL) {
        free(gate);
        return NULL;
    }

    gate->size = size;
    return gate;
}



int valBool(int start, int end, Gate *gate, Circuit *circuit) {
    int value = 0, i = end, exponent = 0;

    do {
        Vari *currentVariable = circuit->variableArray[gate->parameters[i]];

        if (currentVariable->boolean == -1) {
            value = -1;
        } else {
            if (currentVariable->boolean == 1) {
            }

            value += currentVariable->boolean << exponent;
        }

        i--;
        exponent++;

        if (i < start || value == -1) {
            break;  
        }

    } while (1);  

    if (value == -1) {
        
    }

    return value;
}



void addGate(Gate *gate, Circuit *circuit) {
    int n = circuit->numOfGates;
    Gate **gateList = (Gate **)realloc(circuit->gateList, (n + 1) * sizeof(Gate *));

    if (gateList == NULL) {

        fprintf(stderr, "Memory allocation failure.\n");
        exit(EXIT_FAILURE);
    }

    circuit->gateList = gateList;
    circuit->gateList[circuit->numOfGates++] = gate;
}




int evaluateGate(Gate *gate, Circuit *circuit) {
    int output = -1;
    int a, b, holder;

    a = circuit->variableArray[gate->parameters[0]]->boolean;
    b = circuit->variableArray[gate->parameters[1]]->boolean;

    if (gate->type == PASS) {
        
        output = a;
        
        circuit->variableArray[gate->parameters[1]]->boolean = output;
    } else if (gate->type == AND || gate->type == NAND || gate->type == NOR || gate->type == OR || gate->type == XOR) {
        

        if (a != -1 && b != -1) {
            if (gate->type == AND) {
 
            output = a & b;
        } else if (gate->type == OR) {
        
            output = a | b;
        } else if (gate->type == XOR) {
        
            output = a ^ b;
        } else if (gate->type == NAND) {
        
            output = (~(a & b)) & 1;
        } else if (gate->type == NOR) {
        
            output = (~(a | b)) & 1;
        }
    } else {
  
        output = -1;
    }


    circuit->variableArray[gate->parameters[2]]->boolean = output;
    } else if (gate->type == NOT) {
        
        output = (a != -1) ? (~a) & 1 : -1;
        
        circuit->variableArray[gate->parameters[1]]->boolean = output;
    } else if (gate->type == DECODER) {
        
        holder = valBool(0, gate->size - 1, gate, circuit);

        if (holder == -1) {
            int i = 0;
            do {
                circuit->variableArray[gate->parameters[gate->size + i]]->boolean = -1;
                i++;
            } while (i < (1 << gate->size));
        } else {
            int i = 0;
            do {
            circuit->variableArray[gate->parameters[gate->size + i]]->boolean = 0;
            i++;
        } while (i < (1 << gate->size));

        circuit->variableArray[gate->parameters[gate->size + holder]]->boolean = 1;
        output = 1;
    }



    } else if (gate->type == MULTIPLEXER) {
    holder = valBool(1 << gate->size, (1 << gate->size) + gate->size - 1, gate, circuit);
    int output = (holder == -1) ? -1 : circuit->variableArray[gate->parameters[holder]]->boolean;
    circuit->variableArray[gate->parameters[(1 << gate->size) + gate->size]]->boolean = output;
    }


    return output;
}



int incrementInputs(Circuit *circuit) {
    int i = circuit->numOfInputs - 1;

    do {
        if (circuit->variableArray[circuit->inputs[i]]->boolean == 0) {
            circuit->variableArray[circuit->inputs[i]]->boolean = (circuit->variableArray[circuit->inputs[i]]->boolean == 0) ? 1 : circuit->variableArray[circuit->inputs[i]]->boolean;

            return 0;  
        } else {
            circuit->variableArray[circuit->inputs[i]]->boolean = 0;
            i--;
        }
    } while (i >= 0);

    return 1;  
}



void printInputs(Circuit *circuit) {
    int i = 0;

    do {
        printf("%d ", circuit->variableArray[circuit->inputs[i]]->boolean);
        i++;
    } while (i < circuit->numOfInputs);
}


void printOutputs(Circuit *circuit) {
    int i = 0;

    do {
        printf(" %d", circuit->variableArray[circuit->outputs[i]]->boolean);
        i++;
    } while (i < circuit->numOfOutputs);
}


int areOutputsCorrect(Circuit *circuit) {
    int i = 0;

    do {
        if (circuit->variableArray[circuit->outputs[i]]->boolean == -1) {
            return 0;
        }
        i++;
    } while (i < circuit->numOfOutputs);

    return 1;
}


int resetOutputs(Circuit *circuit) {
    int i = 3;  

    do {
        if (circuit->variableArray[i]->out) {
            circuit->variableArray[i]->boolean = -1;
        }
        i++;
    } while (i < circuit->numOfVariables);

    return 0;
}




int simulateCircuit(Circuit *circuit) {
    int num = 0;

    int i = 0;
    do {
        
        circuit->variableArray[circuit->inputs[i]]->boolean = 0;
        i++;
    } while (i < circuit->numOfInputs);

    do {
        printInputs(circuit);
        resetOutputs(circuit);

        while (!areOutputsCorrect(circuit)) {

            int i = 0;
            while (i < circuit->numOfGates) {
                evaluateGate(circuit->gateList[i], circuit);
                i++;
            }
        }


        printf("|");
        printOutputs(circuit);
        printf("\n");

        num = incrementInputs(circuit);
    } while (!num);

    return 0;
}



#define UNKNOWN -1

int getTypeFromName(const char *name) {
    if (strcmp(name, "PASS") == 0) {
        return PASS;
    } else if (strcmp(name, "NOT") == 0) {
        return NOT;
    } else if (strcmp(name, "AND") == 0) {
        return AND;
    } else if (strcmp(name, "OR") == 0) {
        return OR;
    } else if (strcmp(name, "XOR") == 0) {
        return XOR;
    } else if (strcmp(name, "NAND") == 0) {
        return NAND;
    } else if (strcmp(name, "NOR") == 0) {
        return NOR;
    } else if (strcmp(name, "DECODER") == 0) {
        return DECODER;
    } else if (strcmp(name, "MULTIPLEXER") == 0) {
        return MULTIPLEXER;
    } else {
        
        return UNKNOWN;
    }
}





int main(int argc, char **argv) {
    FILE *read;
    char temp[20];
    Circuit *circuit;
    int n, i, j, size, outi, nouts;
    Gate *gate;
    GateType type;


    if (argc < 2) {
    fprintf(stderr, "Error: Insufficient command line arguments.\n");
    fprintf(stderr, "Correct Usage: %s <input_file>\n", argv[0]);
    fprintf(stderr, "Please provide the name of the input file as a command line argument.\n");
    return 1;
}

read = fopen(argv[1], "r");

if (!read) {
    fprintf(stderr, "Error: Unable to open the input file '%s'.\n", argv[1]);
    fprintf(stderr, "Please check if the file exists and you have read permissions.\n");
    return 1;
}



circuit = initializeCircuit();

if (fscanf(read, "%16s %d", temp, &circuit->numOfInputs) != 2) {
    fprintf(stderr, "Error reading input format from the file.\n");
    return 1;
}

if ((circuit->inputs = (int *)malloc(circuit->numOfInputs * sizeof(int))) == NULL) {
    fprintf(stderr, "Error allocating memory for inputs.\n");
    return 1;
}


    i = 0;

    do {
        if (fscanf(read, "%16s", temp) != 1) {
            fprintf(stderr, "Error reading input format from the file.\n");
            return 1;
        }

        n = addVariable(temp, circuit);
        circuit->inputs[i] = n;
        i++;
    } while (i < circuit->numOfInputs);


    if (fscanf(read, "%16s %d", temp, &circuit->numOfOutputs) != 2) {

        fprintf(stderr, "Error reading output format from the file.\n");
        return 1;
    }

    circuit->outputs = (int *)malloc(circuit->numOfOutputs * sizeof(int));
    if (circuit->outputs == NULL) {
        fprintf(stderr, "Error allocating memory for outputs.\n");
        return 1;
    }


    int index = 0;
    do {
        if (fscanf(read, "%16s", temp) != 1) {
            fprintf(stderr, "Error reading output variable from the file.\n");
            return 1;
        }

        n = addVariable(temp, circuit);

        if (index < circuit->numOfOutputs) {
            circuit->outputs[index] = n;
            circuit->variableArray[circuit->outputs[index]]->out = 1;
        } else {
            fprintf(stderr, "Error: Too many output variables specified in the file.\n");
            return 1;
        }

        index++;
    } while (index < circuit->numOfOutputs);


    while (fscanf(read, "%16s", temp) == 1 && !feof(read)) {
    nouts = 1;
    outi = 2;
    size = 0;
    n = 2;

    if (!strcmp(temp, "PASS") || !strcmp(temp, "NOT")) {
        type = (!strcmp(temp, "PASS")) ? PASS : NOT;
        outi = 1;
    } else if (!strcmp(temp, "AND") || !strcmp(temp, "OR") || !strcmp(temp, "XOR") || !strcmp(temp, "NAND") || !strcmp(temp, "NOR")) {
        type = getTypeFromName(temp);
        n = 3;
    } else if (!strcmp(temp, "DECODER") || !strcmp(temp, "MULTIPLEXER")) {
        type = getTypeFromName(temp);
        fscanf(read, "%d", &size);
        n = (type == DECODER) ? size + (1 << size) : size + (1 << size) + 1;
        outi = (type == DECODER) ? size : size + (1 << size);
        nouts = (type == DECODER) ? (1 << size) : 1;
    }

    gate = createNewGate(type, n, size, circuit);

    int d = 0;
    do {
        fscanf(read, "%16s", temp);
        j = addVariable(temp, circuit);
        gate->parameters[d] = j;

        if (d >= outi && d <= outi + nouts - 1) {
            circuit->variableArray[j]->out = 1;
        }

        d++;
    } while (d < n);


    addGate(gate, circuit);
}


    simulateCircuit(circuit);
    freeCircuit(circuit);

    fclose(read);
    return 0;
}