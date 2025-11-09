function createStack() {
  return [];
}
function push(stack, el) {
  stack.push(el);
}
function pop(stack) {
  return stack.pop();
}
function peek(stack) {
  return stack[stack.length - 1];
}
function isEmpty(stack) {
  return stack.length === 0;
}

function createQueue() {
  return [];
}
function enqueue(queue, el) {
  queue.push(el);
}
function dequeue(queue) {
  return queue.shift();
}

function createNode(expr, result) {
  return { expr: expr, result: result, next: null };
}
function addNode(head, expr, result) {
  let node = createNode(expr, result);
  if (head == null) return node;
  let curr = head;
  while (curr.next != null) curr = curr.next;
  curr.next = node;
  return head;
}
function listToArray(head) {
  let arr = [];
  let curr = head;
  while (curr != null) {
    arr.push({ expr: curr.expr, result: curr.result });
    curr = curr.next;
  }
  return arr;
}

function linearSearch(arr, target) {
  for (let i = 0; i < arr.length; i++) {
    if (arr[i].expr === target) return i;
  }
  return -1;
}
function bubbleSort(arr) {
  let n = arr.length;
  for (let i = 0; i < n - 1; i++) {
    for (let j = 0; j < n - i - 1; j++) {
      if (arr[j].result > arr[j + 1].result) {
        let temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  return arr;
}

function showScreen(id) {
  let screens = document.querySelectorAll(".screen");
  for (let i = 0; i < screens.length; i++) {
    screens[i].classList.remove("active");
  }
  document.getElementById(id).classList.add("active");
}

function openSolver(type) {
  currentType = type;
  loadSolver(type);
  showScreen("solver");
}

function fmt(num) {
  if (isNaN(num)) return "NaN";
  let r = Math.round(num * 1e10) / 1e10;
  if (Math.abs(r - Math.round(r)) < 1e-10) return Math.round(r);
  return r.toFixed(6);
}

let currentType = null;
let historyList = null;
let resultQueue = createQueue();

function loadSolver(type) {
  let box = document.getElementById("solverContent");
  if (type === "expr") {
    box.innerHTML =
      "<h2>Expression Solver</h2>" +
      '<input id="exprInput" placeholder="e.g. (3+4)*2^2/(1+1)">' +
      '<br><button onclick="solveExpression()">Solve</button>' +
      '<div id="exprResult"></div>';
  } else if (type === "poly") {
    box.innerHTML =
      "<h2>Quadratic Roots</h2>" +
      '<input id="a" type="number" placeholder="a">' +
      '<input id="b" type="number" placeholder="b">' +
      '<input id="c" type="number" placeholder="c"><br>' +
      '<button onclick="findRoots()">Find Roots</button>' +
      '<div id="polyResult"></div>';
  } else if (type === "eq2") {
    box.innerHTML =
      "<h2>Equation Solver (2 Variables)</h2>" +
      "<p>a₁x + b₁y = c₁<br>a₂x + b₂y = c₂</p>" +
      '<input id="a1" placeholder="a₁">' +
      '<input id="b1" placeholder="b₁">' +
      '<input id="c1" placeholder="c₁"><br>' +
      '<input id="a2" placeholder="a₂">' +
      '<input id="b2" placeholder="b₂">' +
      '<input id="c2" placeholder="c₂"><br>' +
      '<button onclick="solve2x2()">Solve</button>' +
      '<div id="eqResult"></div>';
  }
}

function solveExpression() {
  let expr = document.getElementById("exprInput").value.trim();
  let out = document.getElementById("exprResult");
  try {
    let result = fmt(evaluateExpression(expr));
    out.textContent = "Result: " + result;
    historyList = addNode(historyList, expr, parseFloat(result));
    enqueue(resultQueue, result);
  } catch (e) {
    out.textContent = "Error: " + e.message;
  }
}

function evaluateExpression(s) {
  let tokens = [];
  let pattern = /\s*([0-9]*\.?[0-9]+|\^|\+|\-|\*|\/|\(|\))\s*/g;
  let match;
  while ((match = pattern.exec(s)) !== null) {
    tokens.push(match[1]);
  }
  let prec = { "+": 2, "-": 2, "*": 3, "/": 3, "^": 4, "u-": 5 };
  let right = { "^": true, "u-": true };
  let output = [];
  let ops = createStack();
  for (let i = 0; i < tokens.length; i++) {
    let x = tokens[i];
    if (!isNaN(x)) {
      output.push(x);
      continue;
    }
    if (x == "(") {
      push(ops, x);
      continue;
    }
    if (x == ")") {
      while (!isEmpty(ops) && peek(ops) != "(") {
        output.push(pop(ops));
      }
      if (isEmpty(ops)) throw Error("Mismatched parentheses");
      pop(ops);
      continue;
    }
    if (x == "-") {
      let p = tokens[i - 1];
      if (i == 0 || ["+", "-", "*", "/", "^", "("].includes(p)) x = "u-";
    }
    while (!isEmpty(ops)) {
      let top = peek(ops);
      if (top == "(") break;
      let pt = prec[top] || 0;
      let px = prec[x] || 0;
      if ((right[x] && px < pt) || (!right[x] && px <= pt)) {
        output.push(pop(ops));
      } else break;
    }
    push(ops, x);
  }
  while (!isEmpty(ops)) {
    let o = pop(ops);
    if (o == "(") throw Error("Mismatched parentheses");
    output.push(o);
  }
  let st = createStack();
  for (let i = 0; i < output.length; i++) {
    let x = output[i];
    if (!isNaN(x)) {
      push(st, Number(x));
      continue;
    }
    if (x == "u-") {
      let val = pop(st);
      push(st, -val);
      continue;
    }
    let b = pop(st);
    let a = pop(st);
    if (x == "+") push(st, a + b);
    else if (x == "-") push(st, a - b);
    else if (x == "*") push(st, a * b);
    else if (x == "/") {
      if (b == 0) throw Error("Division by zero");
      push(st, a / b);
    } else if (x == "^") push(st, Math.pow(a, b));
  }
  if (st.length != 1) throw Error("Invalid expression");
  return pop(st);
}

function findRoots() {
  let a = parseFloat(document.getElementById("a").value);
  let b = parseFloat(document.getElementById("b").value);
  let c = parseFloat(document.getElementById("c").value);
  let out = document.getElementById("polyResult");
  if (isNaN(a) || isNaN(b) || isNaN(c)) {
    out.textContent = "Enter all coefficients";
    return;
  }
  let D = b * b - 4 * a * c;
  if (D > 0) {
    let r1 = (-b + Math.sqrt(D)) / (2 * a);
    let r2 = (-b - Math.sqrt(D)) / (2 * a);
    out.textContent = "Two real roots: x₁=" + fmt(r1) + ", x₂=" + fmt(r2);
    historyList = addNode(historyList, a + "x²+" + b + "x+" + c + "=0", r1);
    historyList = addNode(historyList, a + "x²+" + b + "x+" + c + "=0", r2);
  } else if (D == 0) {
    let root = -b / (2 * a);
    out.textContent = "One real root: x=" + fmt(root);
    historyList = addNode(historyList, a + "x²+" + b + "x+" + c + "=0", root);
  } else {
    let real = -b / (2 * a);
    let imag = Math.sqrt(-D) / (2 * a);
    out.textContent = "Complex roots: " + fmt(real) + " ± " + fmt(imag) + "i";
  }
}

function solve2x2() {
  let a1 = parseFloat(document.getElementById("a1").value);
  let b1 = parseFloat(document.getElementById("b1").value);
  let c1 = parseFloat(document.getElementById("c1").value);
  let a2 = parseFloat(document.getElementById("a2").value);
  let b2 = parseFloat(document.getElementById("b2").value);
  let c2 = parseFloat(document.getElementById("c2").value);
  let out = document.getElementById("eqResult");
  if ([a1, b1, c1, a2, b2, c2].some(isNaN)) {
    out.textContent = "Enter all coefficients";
    return;
  }
  let D = a1 * b2 - a2 * b1;
  if (D == 0) {
    out.textContent = "No unique solution";
    return;
  }
  let Dx = c1 * b2 - c2 * b1;
  let Dy = a1 * c2 - a2 * c1;
  let x = Dx / D;
  let y = Dy / D;
  out.textContent = "x=" + fmt(x) + ", y=" + fmt(y);
  historyList = addNode(historyList, a1 + "x+" + b1 + "y=" + c1, x);
}

function showHistoryAnalysis() {
  let arr = listToArray(historyList);
  let sorted = bubbleSort(arr);
  console.log("Sorted results:", sorted);
  console.log("Queue (FIFO results):", resultQueue);
}
