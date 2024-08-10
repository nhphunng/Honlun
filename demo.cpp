#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

struct Employee 
{
    long long id;
    string name;
    double salary;
    string phone;
    string department;
    //Employee(int id, string name, double salary, string phone, string department): id(id), name(name), salary(salary), phone(phone), department(department){}
};

struct AVLTreeNode 
{
    Employee* employee;
    AVLTreeNode* left;
    AVLTreeNode* right;
    int height;
};

struct AVLTree 
{
    AVLTreeNode* root;
};

struct Department 
{
    string name;
    AVLTree employeeTree;
};

AVLTreeNode* createNode(Employee* emp) 
{
    AVLTreeNode* node = new AVLTreeNode();
    node->employee = emp;
    node->left = node->right = nullptr;
    node->height = 1;
    return node;
}

void clearScreen() {
    cout << "\033[2J\033[1;1H"; 
}

int height(AVLTreeNode* node) 
{
    return node ? node->height : 0;
}

int max(int a, int b) 
{
    return (a > b) ? a : b;
}

AVLTreeNode* rightRotate(AVLTreeNode* y) 
{
    AVLTreeNode* x = y->left;
    AVLTreeNode* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

AVLTreeNode* leftRotate(AVLTreeNode* x) 
{
    AVLTreeNode* y = x->right;
    AVLTreeNode* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

int getBalance(AVLTreeNode* node) 
{
    return node ? height(node->left) - height(node->right) : 0;
}

AVLTreeNode* insert(AVLTreeNode* node, Employee* emp) 
{
    if (node == nullptr)
        return createNode(emp);

    if (emp->id < node->employee->id)
        node->left = insert(node->left, emp);
    else if (emp->id > node->employee->id)
        node->right = insert(node->right, emp);
    else
        return node;

    node->height = 1 + max(height(node->left), height(node->right));

    int balance = getBalance(node);

    if (balance > 1 && emp->id < node->left->employee->id)
        return rightRotate(node);

    if (balance < -1 && emp->id > node->right->employee->id)
        return leftRotate(node);

    if (balance > 1 && emp->id > node->left->employee->id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && emp->id < node->right->employee->id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void createAVLTree(AVLTree &tree) 
{
    tree.root = nullptr;
}

void addEmployeeToAVLTree(AVLTree &tree, Employee* emp) {
    tree.root = insert(tree.root, emp);
}

int findDepartmentIndex(const vector<Department>& departments, const string& name) 
{
    for (size_t i = 0; i < departments.size(); i++) {
        if (departments[i].name == name)
            return i;
    }
    return -1;
}

void inOrder(AVLTreeNode* node) 
{
    if (node != nullptr) {
        inOrder(node->left);
        cout << "ID: " << node->employee->id << ", Name: " << node->employee->name
             << ", Salary: " << node->employee->salary << ", Phone: " << node->employee->phone
             << ", Department: " << node->employee->department << endl;
        inOrder(node->right);
    }
}

void displayDepartment(const Department &dept) 
{
    cout << "Department: " << dept.name << endl;
    inOrder(dept.employeeTree.root);
}

void addDepartment(vector<Department> &departments, const string& name) 
{
    if (findDepartmentIndex(departments, name) != -1) {
        cout << "Department already exists!" << endl;
        return;
    }
    Department dept;
    dept.name = name;
    createAVLTree(dept.employeeTree);
    departments.push_back(dept);
}

void editDepartment(vector<Department> &departments, const string& oldName, const string& newName) 
{
    int index = findDepartmentIndex(departments, oldName);
    if (index != -1) {
        departments[index].name = newName;
    } else {
        cout << "Department not found!" << endl;
    }
}

void deleteDepartment(vector<Department> &departments, const string& name) 
{
    int index = findDepartmentIndex(departments, name);
    if (index != -1) {
        departments.erase(departments.begin() + index);
    } else {
        cout << "Department not found!" << endl;
    }
}

void displayAllDepartments(const vector<Department> &departments) 
{
    for (const auto &dept : departments) {
        displayDepartment(dept);
        cout << endl;
    }
}


void addEmployeeToDepartment(vector<Department> &departments, const string& departmentName, Employee* emp) 
{
    int index = findDepartmentIndex(departments, departmentName);
    if (index != -1) {
        addEmployeeToAVLTree(departments[index].employeeTree, emp);
    } else {
        addDepartment(departments, departmentName);
        index = findDepartmentIndex(departments, departmentName);
        addEmployeeToAVLTree(departments[index].employeeTree, emp);
    }
}

AVLTreeNode* minValueNode(AVLTreeNode* node) 
{
    AVLTreeNode* current = node;
    while (current->left != nullptr)
        current = current->left;
    return current;
}

AVLTreeNode* deleteNode(AVLTreeNode* root, int id) 
{
    if (root == nullptr)
        return root;

    if (id < root->employee->id)
        root->left = deleteNode(root->left, id);
    else if (id > root->employee->id)
        root->right = deleteNode(root->right, id);
    else {
        if ((root->left == nullptr) || (root->right == nullptr)) {
            AVLTreeNode* temp = root->left ? root->left : root->right;

            if (temp == nullptr) {
                temp = root;
                root = nullptr;
            } else
                *root = *temp;
            delete temp;
        } else {
            AVLTreeNode* temp = minValueNode(root->right);
            root->employee = temp->employee;
            root->right = deleteNode(root->right, temp->employee->id);
        }
    }

    if (root == nullptr)
        return root;

    root->height = 1 + max(height(root->left), height(root->right));

    int balance = getBalance(root);

    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    if (balance > 1 && getBalance(root->left) < 0) {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    if (balance < -1 && getBalance(root->right) > 0) {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    return root;
}

void deleteEmployee(vector<Department> &departments, const string& departmentName, int empId) 
{
    int index = findDepartmentIndex(departments, departmentName);
    if (index != -1) {
        departments[index].employeeTree.root = deleteNode(departments[index].employeeTree.root, empId);
    } else {
        cout << "Department not found!" << endl;
    }
}

void displayAllEmployeesInDepartment(const vector<Department> &departments, const string& departmentName) 
{
    int index = findDepartmentIndex(departments, departmentName);
    if (index != -1) {
        displayDepartment(departments[index]);
    } else {
        cout << "Department not found!" << endl;
    }
}

void editEmployee(Employee* emp, const string& newName, double newSalary, const string& newPhone) 
{
    emp->name = newName;
    emp->salary = newSalary;
    emp->phone = newPhone;
}

AVLTreeNode* searchEmployee(AVLTreeNode* node, int empId) 
{
    if (node == nullptr || node->employee->id == empId)
        return node;

    if (empId < node->employee->id)
        return searchEmployee(node->left, empId);

    return searchEmployee(node->right, empId);
}

void editEmployeeInDepartment(vector<Department> &departments, const string& departmentName, int empId, const string& newName, double newSalary, const string& newPhone) 
{
    int index = findDepartmentIndex(departments, departmentName);
    if (index != -1) {
        AVLTreeNode* empNode = searchEmployee(departments[index].employeeTree.root, empId);
        if (empNode != nullptr) {
            editEmployee(empNode->employee, newName, newSalary, newPhone);
            cout << "Employee information updated successfully!" << endl;
        } else {
            cout << "Employee not found!" << endl;
        }
    } else {
        cout << "Department not found!" << endl;
    }
}

void readDepartmentFile(vector<Department> &departments)
{
    ifstream fin("department.csv");
    if(!fin)
    {
        cout << "Khong mo duoc file";
        return; 
    }
    string s = "";
    while(fin >> s)
    {
        addDepartment(departments, s);
    }
}

void writeTree(AVLTreeNode* root, ofstream &fout)
{
    if(root == nullptr)
        return;
    else
    {
        // Ghi cây con trái
        writeTree(root->left, fout);
        fout << root->employee->id << ";";
        fout << root->employee->name << ";";
        fout << root->employee->salary << ";";
        fout << root->employee->phone << ";";
        fout << root->employee->department << endl; 
        // Ghi cây con phải
        writeTree(root->right, fout);
    }
}

void writeData(vector<Department> departments)
{
    ofstream fout("employee.csv");
    if(!fout)
    {
        cout << "Khong mo hoac tao duoc file";
        return; 
    }
    fout.clear();
    fout << "ID;Name;Salary;Phone Number;Department" << endl;
    for(auto dep : departments)
    {
        writeTree(dep.employeeTree.root, fout);
    }
    fout.close();
}

void readData(vector<Department> &departments)
{
    ifstream fin("employee.csv");
    if(!fin)
    {
        cout << "Khong mo duoc file";
        return; 
    }
    string s; 
    getline(fin, s);
    while(getline(fin, s))
    {
        stringstream ss(s);
        Employee *emp = new Employee;
        string tmp = "";
        getline(ss, tmp, ';');
        emp->id = stoi(tmp);
        getline(ss, emp->name, ';');
        string tmp2 = "";
        getline(ss, tmp2, ';');
        emp->salary = stod(tmp2);
        getline(ss, emp->phone, ';');
        getline(ss, emp->department);
        addEmployeeToDepartment(departments, emp->department, emp);
    }
    fin.close();
}


void displayMenu() 
{
    cout << "===== HỆ THỐNG QUẢN LÝ NHÂN VIÊN =====" << endl;
    cout << "1. Thêm phòng ban" << endl;
    cout << "2. Sửa phòng ban" << endl;
    cout << "3. Xóa phòng ban" << endl;
    cout << "4. Hiển thị tất cả phòng ban" << endl;
    cout << "5. Thêm nhân viên" << endl;
    cout << "6. Sửa thông tin nhân viên" << endl;
    cout << "7. Xóa nhân viên" << endl;
    cout << "8. Hiển thị tất cả nhân viên trong phòng ban" << endl;
    cout << "9. Hiển thị nhân viên theo mức lương trong phòng ban" << endl;
    cout << "0. Thoát" << endl;
    cout << "Chọn tùy chọn: ";
}

//Them phong ban 
void handleAddDepartment(vector<Department> &departments) 
{
    string name;
    cout << "Nhập tên phòng ban: ";
    cin.ignore();
    getline(cin, name);
    addDepartment(departments, name);
}
//Sua phong ban
void handleEditDepartment(vector<Department> &departments) 
{
    string oldName, newName;
    cout << "Nhập tên phòng ban hiện tại: ";
    cin.ignore();
    getline(cin, oldName);
    cout << "Nhập tên phòng ban mới: ";
    getline(cin, newName);
    editDepartment(departments, oldName, newName);
}
//Xoa phong ban
void handleDeleteDepartment(vector<Department> &departments) 
{
    string name;
    cout << "Nhập tên phòng ban cần xóa: ";
    cin.ignore();
    getline(cin, name);
    deleteDepartment(departments, name);
}
//Hien thi tat ca phong ban 
void handleDisplayAllDepartments(const vector<Department> &departments) 
{
    displayAllDepartments(departments);
}
//Them nhan vien 
void handleAddEmployee(vector<Department> &departments) 
{
     try
    {
        /* code */
        Employee* emp = new Employee;

        cout << "Enter Employee ID: ";
        cin >> emp->id;
        if (cin.fail()) 
        {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard the invalid input
            throw invalid_argument("Employee ID must be an integer!");
        }

        cout << "Enter Employee Name: ";
        cin.ignore();
        getline(cin, emp->name);
        if (emp->name.empty()) 
        {
            throw invalid_argument("Employee Name is empty!");
        }

        cout << "Enter Salary: ";
        cin >> emp->salary;
        if (cin.fail() || emp->salary < 0) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Salary must be a positive real number!");
        }

        cout << "Enter Phone Number: ";
        cin.ignore();
        getline(cin, emp->phone);
        if (emp->phone.empty()) 
        {
            throw invalid_argument("Phone number is empty!");
        }

        cout  << "Enter Department: ";
        getline(cin, emp->department);
        if (emp->department.empty()) 
        {
            throw invalid_argument("Department is empty!");
        }

        addEmployeeToDepartment(departments, emp->department, emp);
        cout << "Employee Added Successfully!" << endl; 
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        cout << "Please re-enter!" << endl;
        handleAddEmployee(departments);
    } 
}

//Sua thong tin nhan vien
void handleEditEmployee(vector<Department> &departments) 
{
     try
    {

        int id;
        string department, newName, newPhone;
        double newSalary;
        
        cout << "Enter Department: ";
        cin.ignore();
        getline(cin, department);
        if(department.empty())
        {
            throw invalid_argument("Department name is empty!");
        }

        cout << "Enter Employee ID: ";
        cin >> id;
        if (cin.fail()) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Bo qua input sai 
            throw invalid_argument("Employee ID must be an integer!");
        }


        cout << "Enter New Employee Name: ";
        cin.ignore();
        getline(cin, newName);
        if (newName.empty()) 
        {
            throw invalid_argument("Employee Name is empty!");
        }

        cout << "Enter New Salary: ";
        cin >> newSalary;
        if (cin.fail() || newSalary < 0) 
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            throw invalid_argument("Salary must be a positive real number!");
        }

        cout << "Enter Employee Phone Number: ";
        cin.ignore();
        getline(cin, newPhone);
         if (newPhone.empty()) 
        {
            throw invalid_argument("Phone number is empty!");
        }

        editEmployeeInDepartment(departments, department, id, newName, newSalary, newPhone);
        cout << "Employee Information Update Successfully" << endl;
    }
    catch (const invalid_argument &e)
    {
        cout << "Error: " << e.what() << endl;
        cout << "Please re-enter!" << endl;
        handleEditEmployee(departments);
    }
}

void handleDeleteEmployee(vector<Department> &departments) 
{
    int id;
    string department;
    
    cout << "Nhập tên phòng ban: ";
    cin.ignore();
    getline(cin, department);
    cout << "Nhập mã nhân viên cần xóa: ";
    cin >> id;
    
    deleteEmployee(departments, department, id);
}

void handleDisplayAllEmployeesInDepartment(const vector<Department> &departments) 
{
    string department;
    cout << "Nhập tên phòng ban: ";
    cin.ignore();
    getline(cin, department);
    displayAllEmployeesInDepartment(departments, department);
}

void handleDisplayEmployeesBySalaryInDepartment(const vector<Department> &departments) 
{
    string department;
    cout << "Nhập tên phòng ban: ";
    cin.ignore();
    getline(cin, department);
    cout << "Nhân viên được sắp xếp theo mức lương:" << endl;
    // Implement function to display employees by increasing salary here
}

int main() {
    vector<Department> departments;
    readData(departments);
    int choice;

    do {
        displayMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                handleAddDepartment(departments);
                break;
            case 2:
                handleEditDepartment(departments);
                break;
            case 3:
                handleDeleteDepartment(departments);
                break;
            case 4:
                handleDisplayAllDepartments(departments);
                break;
            case 5:
                handleAddEmployee(departments);
                break;
            case 6:
                handleEditEmployee(departments);
                break;
            case 7:
                handleDeleteEmployee(departments);
                break;
            case 8:
                handleDisplayAllEmployeesInDepartment(departments);
                break;
            case 9:
                handleDisplayEmployeesBySalaryInDepartment(departments);
                break;
            case 0:
                cout << "Đang thoát chương trình." << endl;
                break;
            default:
                cout << "Lựa chọn không hợp lệ. Vui lòng thử lại." << endl;
        }
        
        cout << endl;

    } while (choice != 0);
    writeData(departments);
    return 0;
}



