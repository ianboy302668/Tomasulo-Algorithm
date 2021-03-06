# Tomasulo Algorithm
## 專案說明
  此為計算機組織程式作業
  給定一組合語言之程式碼，輸出所有有變化的cycle
  Input ：一組合語言，使用者可指定ADD/SUB/MUL/DIV在ALU中需執行幾個cycle
  Output：輸出所有有變化cycle，包括目前cycle/RF/RAT/RS/buffer status
## 功能說明
**基本流程:**
  1. 輸入各opcode需執行的cycle量 及 RF初始值
  2. 輸入程式碼
  3. 將每行instruction的資訊用Rs結構內的各變數承接
  4. 依序執行issue -> dispatch -> write
  5. 輸出結果
  6. 重複執行直到所有程式碼皆寫入
## 輸入範例
 *需執行的cycle*

        2
        1
        4
        8
 *RF初始值*

        0 
        2 
        4 
        6 
        8 
 *Assembly code*

        ADDI F1, F2, 1
        SUB F1, F3, F4
        DIV F1, F2, F3
        MUL F2, F3, F4
        ADD F2, F4, F2
        ADDI F4, F1, 2
        MUL F5, F5, F5
        ADD F1, F4, F4

## 程式說明
 * <font color=#00FF00>RS</font> struct 
 
    `busy` 表該RS是否有程式正在執行
    
    `inst_operator` 表此RS當前儲存之operator為四則運算中其一
   
    `destination` 同組合語言中的rd，用於儲存目標位置
    
    `resource1` `resource2` 同組合語言中的rs，用於儲存來源位置
    
    `is_rat_i` `is_rat_j` true: 為儲存於RAT的RS之index `非一般數字` /  false: 為一般數字
    
 * <font color=#00FF00>Alu</font>  struct
 
    `int cycle` 該程式應結束的cycle
    
    `int rs_index` RS的index
    
    `bool is_empty` Buffer內是否有成是佔著

## 輸出範例
        Cycle 1

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS1 |
    F2 |      |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |   + |     2 |     1 |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |     |       |       |
    RS5 |     |       |       |
        -----------------------
    BUFFER: Empty

    Cycle 2

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS2 |
    F2 |      |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |   + |     2 |     1 |
    RS2 |   - |     4 |     6 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS1) 2 + 1
        -----------------------
    RS4 |     |       |       |
    RS5 |     |       |       |
        -----------------------
    BUFFER: Empty

    Cycle 3

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS4 |
    F2 |      |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |     |       |       |
    RS2 |   - |     4 |     6 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS2) 4 - 6
        -----------------------
    RS4 |   / |     2 |     4 |
    RS5 |     |       |       |
        -----------------------
    BUFFER: Empty

    Cycle 4

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS4 |
    F2 |  RS5 |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |     |       |       |
    RS2 |   - |     4 |     6 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS2) 4 - 6
        -----------------------
    RS4 |   / |     2 |     4 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS4) 2 / 4

    Cycle 5

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS4 |
    F2 |  RS1 |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |   / |     2 |     4 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS4) 2 / 4

    Cycle 6

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS4 |
    F2 |  RS1 |
    F3 |      |
    F4 |  RS2 |
    F5 |      |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |   + |   RS4 |     2 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |   / |     2 |     4 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS4) 2 / 4

    Cycle 12

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   6 |
    F5 |   8 |
    -------


    - RAT --
    F1 |      |
    F2 |  RS1 |
    F3 |      |
    F4 |  RS2 |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |   + |     0 |     2 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS2) 0 + 2
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS5) 4 * 6

    Cycle 13

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   2 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS2 |
    F2 |  RS1 |
    F3 |      |
    F4 |      |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |   + |     2 |     2 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS5) 4 * 6

    Cycle 14

    - RF --
    F1 |   0 |
    F2 |   2 |
    F3 |   4 |
    F4 |   2 |
    F5 |   8 |
    -------


    - RAT --
    F1 |  RS2 |
    F2 |  RS1 |
    F3 |      |
    F4 |      |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |   + |     2 |     2 |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS2) 2 + 2
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS5) 4 * 6

    Cycle 15

    - RF --
    F1 |   4 |
    F2 |   2 |
    F3 |   4 |
    F4 |   2 |
    F5 |   8 |
    -------


    - RAT --
    F1 |      |
    F2 |  RS1 |
    F3 |      |
    F4 |      |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |   + |     6 |   RS5 |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |   * |     4 |     6 |
        -----------------------
    BUFFER: (RS5) 4 * 6

    Cycle 16

    - RF --
    F1 |   4 |
    F2 |   2 |
    F3 |   4 |
    F4 |   2 |
    F5 |   8 |
    -------


    - RAT --
    F1 |      |
    F2 |  RS1 |
    F3 |      |
    F4 |      |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |   + |     6 |    24 |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: (RS1) 6 + 24
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |     |       |       |
        -----------------------
    BUFFER: (RS4) 8 * 8

    Cycle 17

    - RF --
    F1 |   4 |
    F2 |  30 |
    F3 |   4 |
    F4 |   2 |
    F5 |   8 |
    -------


    - RAT --
    F1 |      |
    F2 |      |
    F3 |      |
    F4 |      |
    F5 |  RS4 |
    --------


        - RS ------------------
    RS1 |     |       |       |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |   * |     8 |     8 |
    RS5 |     |       |       |
        -----------------------
    BUFFER: (RS4) 8 * 8

    Cycle 20

    - RF --
    F1 |   4 |
    F2 |  30 |
    F3 |   4 |
    F4 |   2 |
    F5 |  64 |
    -------


    - RAT --
    F1 |      |
    F2 |      |
    F3 |      |
    F4 |      |
    F5 |      |
    --------


        - RS ------------------
    RS1 |     |       |       |
    RS2 |     |       |       |
    RS3 |     |       |       |
        -----------------------
    BUFFER: Empty
        -----------------------
    RS4 |     |       |       |
    RS5 |     |       |       |
        -----------------------
    BUFFER: Empty
