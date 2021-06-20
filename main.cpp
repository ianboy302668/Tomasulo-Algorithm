#include <iostream>
#include <vector>
#include <queue>
#include <sstream>
#include <string>
#include <iomanip>

//#define sub = 2, add = 1, mul = 3, div = 4; //the cycle cost of each operator

using namespace std;

typedef struct
{
    bool busy = false;
    char inst_operator = ' ';
    int destination = -1;
    double resource1 = -1, resource2 = -1;
    bool is_rat_i = false, is_rat_j = false; // true: is a string of rat,  false: is a number
} Rs;

typedef struct
{
    int cycle;
    int rs_index;
    bool is_empty = true;
} Alu;

int sub, add, mul, division; //the cycle cost of each operator
int current_cycle = 0;
bool changed_cycle = false;
Alu bufferADD, bufferMUL;
queue<Rs> q;
vector<Rs> rs(5);
vector<double> rf(5), rat(5, -1);

void issue();
void dispatch();
void write_result(Alu &);
void print();
int integer_trasition(string);

int main()
{
    int t;
    string code, res1, res2, des;
    stringstream ss;
    bool is_difference = false;

    cout << "Please enter the cycle cost for each instraction\n";
    cout << "SUB: ";
    cin >> sub;
    cout << "ADD: ";
    cin >> add;
    cout << "MUL: ";
    cin >> mul;
    cout << "DIV: ";
    cin >> division;

    // * initial the RF * //
    cout << "Please enter some numbers for initializing RF\n";
    for (int i = 0; i < 5; i++)
    {
        cout << "F" << i + 1 << ":";
        cin >> t;
        rf.at(i) = t;
    }

    // * input *//
    cout << "Enter the instruction for input\n";
    Rs temp;
    while (getline(cin, code))
    {
        if (!current_cycle)
        {
            current_cycle++;
            continue;
        }

        ss << code; // it's more convinent for processing data later
        ss >> code >> des >> res1 >> res2;
        //cout << "check: " << code << des << res1 << res2 << endl;  //debug

        temp.destination = integer_trasition(des) - 1;
        temp.resource1 = integer_trasition(res1) - 1.0;
        temp.is_rat_i = true;
        if (code != "ADDI")
        {
            temp.resource2 = integer_trasition(res2) - 1.0;
            temp.is_rat_j = true;
        }
        else
            temp.resource2 = (double)stoi(res2);

        // * classify Inst. type * //
        if (code == "SUB")
        {
            temp.inst_operator = '-';
        }
        else if (code == "DIV")
        {
            temp.inst_operator = '/';
        }
        else if (code == "MUL")
        {
            temp.inst_operator = '*';
        }
        else if (code == "ADD")
        {
            temp.inst_operator = '+';
        }
        else
            temp.inst_operator = '&';

        // * initial queue * //
        q.push(temp);

        // * initialize stringstream * //
        ss.str();
        ss.clear();
    }

    do
    {
        if (q.empty())
        {
            int i;
            for (i = 0; i < 5; i++)
            {
                if (rs.at(i).busy)
                    break;
            }
            if (i == 5)
                break;
        }

        // * processing * //
        write_result(bufferADD);
        write_result(bufferMUL);
        dispatch();
        issue();

        // * output * //
        if (changed_cycle)
            print();

        current_cycle++;
    } while (1);
}

void issue()
{
    // * check the rs if empty * //
    if (!q.empty())
    {
        Rs target = q.front();

        switch (target.inst_operator)
        {
        case '&': //addi
        case '+':
        case '-':
        {
            for (int i = 0; i < 3; i++)
            {
                if (!rs[i].busy)
                { //it's empty

                    // * check rs1 * //
                    if (rat[target.resource1] == -1)
                    { //is nothing
                        rs[i].resource1 = rf[target.resource1];
                    }
                    else
                    { // it's somthing there
                        rs[i].resource1 = rat[target.resource1];
                        rs[i].is_rat_i = true;
                    }

                    // * check rs2 * //
                    if (target.inst_operator == '&')
                        rs[i].resource2 = target.resource2;
                    else if (rat[target.resource2] == -1)
                    { //is nothing
                        rs[i].resource2 = rf[target.resource2];
                    }
                    else
                    { // it's somthing there
                        rs[i].resource2 = rat[target.resource2];
                        rs[i].is_rat_j = true;
                    }

                    rat[target.destination] = i; //rs[0:2]: add  rs[3:4]: mul

                    rs[i].inst_operator = target.inst_operator;
                    if (target.inst_operator == '&')
                        rs[i].inst_operator = '+';

                    // * update the rs status * //
                    rs[i].busy = true;
                    changed_cycle = true;
                    q.pop();
                    break;
                }
                //end for
            }
            break; //end case
        }
        // * div & mul * //
        default:
            for (int i = 3; i < 5; i++)
            {
                if (!rs[i].busy)
                { //it's empty

                    // * check rs1 * //
                    if (rat[target.resource1] == -1)
                    { //is nothing
                        rs[i].resource1 = rf[target.resource1];
                    }
                    else
                    { // it's somthing there
                        rs[i].resource1 = rat[target.resource1];
                        rs[i].is_rat_j = true;
                    }

                    // * check rs2 * //
                    if (rat[target.resource2] == -1)
                    { //is nothing
                        rs[i].resource2 = rf[target.resource2];
                    }
                    else
                    { // it's somthing there
                        rs[i].resource2 = rat[target.resource2];
                        rs[i].is_rat_j = true;
                    }

                    rs[i].inst_operator = target.inst_operator;

                    // * update the rat * //
                    rat[target.destination] = i; //rs[0:2]: add  rs[3:4]: mul

                    // * update the rs status * //
                    rs[i].busy = true;
                    changed_cycle = true;
                    q.pop();
                    break; //end for
                }
            }
            break; //end case
        }
    }
}

void dispatch()
{
    // * add unit * //
    if (bufferADD.is_empty)
    {
        for (int i = 0; i < 3; i++)
        {
            if (rs[i].busy)
            {
                if (!rs[i].is_rat_i && !rs[i].is_rat_j)
                { //both i and j is number instead the varaiable in the RAT

                    // * update the buffer * //
                    bufferADD.rs_index = i;
                    bufferADD.is_empty = false;

                    if (rs[i].inst_operator == '+')
                        bufferADD.cycle = current_cycle + add;
                    else
                        bufferADD.cycle = current_cycle + sub;

                    // * light the changed_cycle * //
                    changed_cycle = true;
                }
            }
        }
    }

    // * mul unit * //
    if (bufferMUL.is_empty)
    {
        for (int i = 3; i < 5; i++)
        {
            if (rs[i].busy)
            {
                if (!rs[i].is_rat_i && !rs[i].is_rat_j)
                { //both i and j is number instead the varaiable in the RAT

                    // * update the buffer * //
                    bufferMUL.rs_index = i;
                    bufferMUL.is_empty = false;

                    if (rs[i].inst_operator == '*')
                        bufferMUL.cycle = current_cycle + mul;
                    else
                        bufferMUL.cycle = current_cycle + division;

                    // * light the changed_cycle * //
                    changed_cycle = true;
                }
            }
        }
    }
}

void write_result(Alu &buffer)
{
    if (current_cycle == buffer.cycle)
    {
        int index = buffer.rs_index;
        int result;

        // * calculate the result * //
        if (rs[index].inst_operator == '+')
            result = rs[index].resource1 + rs[index].resource2;
        else if (rs[index].inst_operator == '-')
            result = rs[index].resource1 - rs[index].resource2;
        else if (rs[index].inst_operator == '*')
            result = rs[index].resource1 * rs[index].resource2;
        else
            result = rs[index].resource1 / rs[index].resource2;

        buffer.is_empty = true;

        // * update RF & RAT * //

        for (int i = 0; i < 5; i++)
        {
            if (rat.at(i) == index)
            {
                rat[i] = -1;
                rf[i] = result;
            }
        }

        // * capture * //
        for (int i = 0; i < 5; i++)
        {
            if (rs[i].busy)
            {
                if (rs[i].is_rat_i && rs[i].resource1 == index)
                {
                    rs[i].resource1 = result;
                    rs[i].is_rat_i = false;
                }

                if (rs[i].is_rat_j && rs[i].resource2 == index)
                {
                    rs[i].resource2 = result;
                    rs[i].is_rat_j = false;
                }
            }
        }

        // * release RS * //
        rs[index].busy = rs[index].is_rat_i = rs[index].is_rat_j = false;
        rs[index].destination = rs[index].resource1 = rs[index].resource2 = -1;
        rs[index].inst_operator = ' ';

        changed_cycle = true;
    }
}

void print()
{
    cout << "\nCycle " << current_cycle << "\n\n";

    // * RF * //
    cout << "   - RF --\n";
    for (int i = 0; i < 5; i++)
    {
        cout << "F" << i + 1 << " |" << setw(4) << rf.at(i) << " |\n";
    }
    cout << "   -------\n\n\n";

    // * RAT * //
    cout << "   - RAT --\n";
    for (int i = 0; i < 5; i++)
    {
        cout << "F" << i + 1 << " | ";
        if (rat.at(i) == -1)
            cout << "     |\n";
        else
            cout << " RS" << rat.at(i) + 1 << " |\n";
    }
    cout << "   --------\n\n\n";

    // * RS * //
    cout << "    - RS ------------------\n";
    for (int i = 0; i < 3; i++)
    {
        cout << "RS" << i + 1 << " |" << setw(4) << rs.at(i).inst_operator << " |";

        // res1
        if (rs.at(i).is_rat_i)
            cout << setw(5) << "RS" << rs.at(i).resource1 + 1 << " |";
        else
        {
            if (rs.at(i).resource1 == -1)
                cout << setw(6) << ' ' << " |";
            else
                cout << setw(6) << rs.at(i).resource1 << " |";
        }

        // res2
        if (rs.at(i).is_rat_j)
            cout << setw(5) << "RS" << rs.at(i).resource2 + 1 << " |\n";
        else
        {
            if (rs.at(i).resource2 == -1)
                cout << setw(6) << ' ' << " |\n";
            else
                cout << setw(6) << rs.at(i).resource2 << " |\n";
        }

        // * print the buffer status * //
        if (i == 2)
        {
            cout << "    -----------------------\n";

            cout << "BUFFER: ";
            if (!bufferADD.is_empty)
                cout << "(RS" << bufferADD.rs_index + 1 << ") "
                     << rs[bufferADD.rs_index].resource1 << ' ' << rs[bufferADD.rs_index].inst_operator << ' ' << rs[bufferADD.rs_index].resource2 << endl;
            else
                cout << "Empty\n";
        }
    }

    cout << "    -----------------------\n";
    for (int i = 3; i < 5; i++) // 0-2: add  3-4:mul
    {
        cout << "RS" << i + 1 << " |" << setw(4) << rs.at(i).inst_operator << " |";

        // res1
        if (rs.at(i).resource1 == -1)
            cout << setw(6) << ' ' << " |";
        else
            cout << setw(6) << rs.at(i).resource1 << " |";

        // res2
        if (rs.at(i).resource2 == -1)
            cout << setw(6) << ' ' << " |\n";
        else
            cout << setw(6) << rs.at(i).resource2 << " |\n";

        // * print the buffer status * //
        if (i == 4)
        {
            cout << "    -----------------------\n";

            cout << "BUFFER: ";
            if (!bufferMUL.is_empty)
                cout << "(RS" << bufferMUL.rs_index + 1 << ") "
                     << rs[bufferMUL.rs_index].resource1 << ' ' << rs[bufferMUL.rs_index].inst_operator << ' ' << rs[bufferMUL.rs_index].resource2 << endl;
            else
                cout << "Empty\n";
        }
    }

    changed_cycle = false; //initialize
}

int integer_trasition(string target)
{
    size_t pos = target.find(",");
    if (pos == string::npos)
    {
        target = target.substr(1, pos);
        return stoi(target);
    }
    else
    {
        target = target.substr(1, pos - 1);
        return stoi(target);
    }
}
