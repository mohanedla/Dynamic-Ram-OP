#include <iostream>
#include <string>
using namespace std;
int ram, os;
struct times
{
    int* exc, * runtime;
};
struct operations
{
    string proc;
    int size;
    times time;
};
struct dynamic
{
    string proc;
    bool used = true;
    int begin, end, size, exc, runtime;
    dynamic* prev = NULL;
    dynamic* next = NULL;
};
struct waiting
{
    string proc;
    int size, time;
    waiting* prev = NULL;
    waiting* next = NULL;
};
dynamic* head = NULL;
waiting* front = NULL;
void create(string proc, int size, int exc, int runtime)
{
    dynamic* node;
    if (head == NULL)
    {
        node = new dynamic;
        node->proc = "OS";
        node->size = os;
        node->begin = 0;
        node->end = os;
        head = node;
        node = new dynamic;
        node->proc = proc;
        node->exc = exc;
        node->size = size;
        node->runtime = runtime;
        node->begin = os ;
        node->end = node->begin + size;
        head->next = node;
        node->prev = head;
        head = node;
    }
    else
    {
        node = new dynamic;
        node->proc = proc;
        node->exc = exc;
        node->size = size;
        node->runtime = runtime;

        dynamic* temp = head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        node->begin = temp->end;
        node->end = node->begin + size;
        temp->next = node;
        node->prev = temp;
    }
}
int fragmentsisfull() {
    if (head == NULL) return 0;
    dynamic* temp = head;
    int sum = 0;
    while (temp->next != NULL) {
        if (temp->used == false) {
            sum += temp->size;
        }
        temp = temp->next;
    }
    if (temp->used == false) {
        sum += temp->size;
    }
    return sum;
}
void updatefragments(string proc, int size, int exc, int runtime);
void shiftfragments(string proc, int size, int exc, int runtime) {
    int tsize;
    dynamic* temp = head;
    dynamic* temp1 = head;
    while (temp->next != NULL) {
        tsize = 0;
        if (temp->used == false) {
            if (temp->next->used == false) {
                temp->size = temp->size + temp->next->size;
                temp->end = temp->begin + temp->size;
                if (temp->next->next == NULL) {
                    temp->next = NULL;
                }
                else {
                    temp1 = temp;
                    temp = temp->next;
                    temp->prev->next = temp->next;
                    temp->next->prev = temp->prev;
                    if (temp1->size >= size) {
                        cout << "hello";
                        updatefragments(proc, size, exc, runtime);
                        return;
                    }
                    temp = temp1->next;
                }

            }
            else {
                tsize = temp->size;
                temp->proc = temp->next->proc;
                temp->exc = temp->next->exc;
                temp->runtime = temp->next->runtime;
                temp->end = temp->begin + temp->next->size;
                temp->size = temp->next->size;
                temp->used = temp->next->used;
                temp->next->proc = "";
                temp->next->exc = 0;
                temp->next->runtime = 0;
                temp->next->size = tsize;
                temp->next->begin = temp->end;
                temp->next->end = temp->next->begin + temp->next->size;
                temp->next->used = false;
                temp = temp->next;
            }
        }
        else
            temp = temp->next;
        
    }
    temp = head;
    while (temp->used == true && temp->next != NULL) {
        temp = temp->next;
    }
    if (temp->used == false) {
        temp->size = fragmentsisfull();
        temp->end = temp->begin + temp->size;
        if (temp->size >= size) {
            if (temp->size == size) {
                temp->proc = proc;
                temp->exc = exc;
                temp->runtime = runtime;
                temp->used = true;
            }
            else if (temp->size > size) {
                dynamic* node = new dynamic;
                node->size = temp->size - size;
                temp->proc = proc;
                temp->exc = exc;
                temp->runtime = runtime;
                temp->size = size;
                temp->end = temp->begin + temp->size;
                temp->used = true;
                node->begin = temp->end;
                node->end = node->begin + node->size;
                node->used = false;
                temp->next = node;
                node->prev = temp;
            }
        }
        else {
            if (temp->size + ram == size) {
                temp->proc = proc;
                temp->exc = exc;
                temp->runtime = runtime;
                temp->size = size;
                temp->end = temp->begin + temp->size;
                temp->used = true;
            }
            else if (temp->size + ram > size) {
                dynamic* node = new dynamic;
                node->size = (temp->size + ram) - size;
                temp->proc = proc;
                temp->exc = exc;
                temp->runtime = runtime;
                temp->size = size;
                temp->end = temp->begin + temp->size;
                temp->used = true;
                node->begin = temp->end;
                node->end = node->begin + node->size;
                node->used = false;
                temp->next = node;
                node->prev = temp;
            }
            ram = 0;
        }
    }
}
void updatefragments(string proc, int size, int exc, int runtime) {
    dynamic* temp = head;
    if (fragmentsisfull() >= size) {
        while (temp != NULL) {
            if (temp->used == false) {
                if (temp->size == size) {
                    temp->proc = proc;
                    temp->exc = exc;
                    temp->runtime = runtime;
                    temp->used = true;
                    return;
                }
                else if (temp->size > size) {
                    dynamic* node = new dynamic;
                    node->size = temp->size - size;
                    temp->proc = proc;
                    temp->exc = exc;
                    temp->runtime = runtime;
                    temp->size = size;
                    temp->end = temp->begin + temp->size;
                    temp->used = true;
                    node->begin = temp->end;
                    node->end = node->begin + node->size;
                    node->used = false;
                    if (temp->next != NULL) {
                        node->next = temp->next;
                        node->prev = temp;
                        temp->next->prev = node;
                        temp->next = node;
                    }
                    else {
                        temp->next = node;
                        node->prev = temp;
                    }
                    return;
                }
            }
            temp = temp->next;
        }
            shiftfragments(proc, size, exc, runtime);
    }
}
void enqueue(string proc, int size, int exc, int runtime)
{
    waiting* node = new waiting;
    node->proc = proc;
    node->size = size;
    node->time = runtime - exc;
    if (front == NULL) {
        front = node;
    }
    else {
        waiting* temp = front;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = node;
        node->prev = temp;
    }
}
void dequeue(int sec)
{
    if (front == NULL)
        return;
    else if (front->next ==NULL && front->size <= fragmentsisfull())
    {
        updatefragments(front->proc, front->size, sec, front->time + sec);
        front = NULL;
    }
    else {
        waiting* temp = front;
        waiting* del;
        while (temp->next != NULL) {
            if (temp->size <= ram + fragmentsisfull()) {
                updatefragments(temp->proc, temp->size, sec, temp->time + sec);
                del = temp;
                if (temp == front) {
                    front = front->next;
                    front->prev = NULL;
                    temp = front;
                }
                else {
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
                temp = temp->next;
                }
                delete (del);
            }
             
        }
        if (temp->size <= ram + fragmentsisfull()) {
            updatefragments(temp->proc, temp->size, sec, temp->time + sec);
            temp->prev->next = NULL;
            delete (temp);
        }
    }
}
void secondwaiting(int &max) {
    if (front == NULL) return ;
    else {
        
        waiting* temp = front;
        while (temp->next != NULL) {
            if (temp->time > max) {
                max = temp->time;
            }
            temp = temp->next;
        }
        if (temp->time > max) {
            max = temp->time;
        }
    }
}
void Delete(string proc, int sec)
{
    if (head == NULL) return;
    dynamic* temp = head;
    while (temp->proc != proc && temp->next != NULL) {
        temp = temp->next;
    }
    if (temp->proc == proc) {
        temp->proc = "";
        temp->exc = 0;
        temp->runtime = 0;
        temp->used = false;
        dequeue(sec);
    }
}
void check(int sec)
{
     if (head->runtime == sec && head->used == true) {
        Delete(head->proc, sec);
    }
    else {
        dynamic* temp = head;
        while (temp->next != NULL) {
            if (temp->runtime == sec && temp->used == true) {
                Delete(temp->proc, sec);
            }
            temp = temp->next;
        }
        if (temp->runtime == sec && temp->used == true) {
            Delete(temp->proc, sec);
        }
    }
}
void display()
{
    if (head == NULL) {
        cout << "Ram Is Empty!\n"; return;
    }
    dynamic* temp = head;
    cout << " | " << head->prev->begin << " | " << head->prev->proc << " | " << head->prev->end << " | " << "->";
    while (temp->next != NULL)
    {
        cout << " | " << temp->begin << " | " << temp->proc << " | " << temp->end << " | " << "->";
        temp = temp->next;
    }
    cout << " | " << temp->begin << " | " << temp->proc << " | " << temp->end << " | ->";
    cout << "|" << temp->end << " | RAM |" << temp->end+ram << " |\n";
}
void displaywaiting() {
    if (front == NULL) {
        cout << "There Isn't Operation Is Waiting:) \n"; return;
    }
    waiting* temp = front;
    while (temp->next != NULL) {
        cout << " | " << temp->proc << " | " << "->";
        temp = temp->next;
    }cout << " | " << temp->proc << " | ";

}
int max(operations* op, int* m, int n) {
    int max = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m[i]; j++) {
            if (op[i].time.runtime[j] > max) {
                max = op[i].time.runtime[j];
            }
        }
    }
    return max;
}
int main()
{
    int count = 0;
    cout << "Enter Size Of Ram:\t";
    cin >> ram;
    int n, * m;
    cout << "Enter n Of Process:\t";
    cin >> n;
    m = new int[n];
    cout << "Enter size Of OS:\t";
    cin >> os;
    ram -= os;
    operations* op;
    op = new operations[n];
    for (int i = 0; i < n; i++)
    {
        cout << "Enter Proceser[" << i + 1 << "]=>\t";
        cin.ignore();
        getline(cin, op[i].proc);
        cout << "Enter size[" << i + 1 << "]=>\t";
        cin >> op[i].size;
        cout << "how many time want to insert for this opertion:\t";
        cin >> m[i];
        op[i].time.exc = new int[m[i]];
        op[i].time.runtime = new int[m[i]];
        for (int j = 0; j < m[i]; j++)
        {
            cout << "Enter Excute[" << j + 1 << "]=>\t";
            cin >> op[i].time.exc[j];
            cout << "Runtime Proceser[" << j + 1 << "]=>\t";
            cin >> op[i].time.runtime[j];
        }
    }
    cout << endl;
    dynamic* node;
    int MAX = 0;
    
    for (int i = 0; i <= max(op, m, n) + MAX; i++)
    {
        for (int j = 0; j < n; j++)
        {
        if (head != NULL) check(i);
            for (int k = 0; k < m[j]; k++)
            {
                if (op[j].time.exc[k] == i)
                {
                    if (fragmentsisfull() >= op[j].size)
                    {
                        updatefragments(op[j].proc, op[j].size, op[j].time.exc[k], op[j].time.runtime[k]);
                    }
                    else if (ram >= op[j].size) {
                        create(op[j].proc, op[j].size, op[j].time.exc[k], op[j].time.runtime[k]);
                        ram -= op[j].size;
                        
                    }
                    else if ((fragmentsisfull() + ram) >= op[j].size) {
                        shiftfragments(op[j].proc, op[j].size, op[j].time.exc[k], op[j].time.runtime[k]);
                    }
                    else
                    {
                        enqueue(op[j].proc, op[j].size, op[j].time.exc[k], op[j].time.runtime[k]);
                    }
                }

            }
        }
        cout << i << "=>\t";
        display();
        displaywaiting();
        cout << "\n--------------------------------------------------------------------\n";
        secondwaiting(MAX);
    }
}
