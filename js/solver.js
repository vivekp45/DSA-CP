function showScreen(id){
  document.querySelectorAll('.screen').forEach(s=>s.classList.remove('active'));
  document.getElementById(id).classList.add('active');
}
function openSolver(type){
  currentType=type;
  loadSolver(type);
  showScreen('solver');
}
function fmt(num){
  if(Number.isNaN(num))return"NaN";
  const r=Math.round(num*1e10)/1e10;
  return Math.abs(r-Math.round(r))<1e-10?Math.round(r):r.toFixed(6);
}
let currentType=null;
function loadSolver(type){
  const box=document.getElementById("solverContent");
  if(type==="expr"){
    box.innerHTML=`<h2>Expression Solver</h2>
      <input id="exprInput" placeholder="e.g. (3+4)*2^2/(1+1)">
      <br><button onclick="solveExpression()">Solve</button>
      <div id="exprResult"></div>`;
  }else if(type==="poly"){
    box.innerHTML=`<h2>Quadratic Roots</h2>
      <input id="a" type="number" placeholder="a">
      <input id="b" type="number" placeholder="b">
      <input id="c" type="number" placeholder="c"><br>
      <button onclick="findRoots()">Find Roots</button>
      <div id="polyResult"></div>`;
  }else if(type==="eq2"){
    box.innerHTML=`<h2>Equation Solver (2 Variables)</h2>
      <p>a₁x + b₁y = c₁<br>a₂x + b₂y = c₂</p>
      <input id="a1" placeholder="a₁">
      <input id="b1" placeholder="b₁">
      <input id="c1" placeholder="c₁"><br>
      <input id="a2" placeholder="a₂">
      <input id="b2" placeholder="b₂">
      <input id="c2" placeholder="c₂"><br>
      <button onclick="solve2x2()">Solve</button>
      <div id="eqResult"></div>`;
  }
}
function solveExpression(){
  const expr=document.getElementById("exprInput").value.trim();
  const out=document.getElementById("exprResult");
  try{out.textContent="Result: "+fmt(evaluateExpression(expr));}
  catch(e){out.textContent="Error: "+e.message;}
}
function evaluateExpression(s){
  const t=[];const re=/\s*([0-9]*\.?[0-9]+|\^|\+|\-|\*|\/|\(|\))\s*/g;let m;
  while((m=re.exec(s))!==null)t.push(m[1]);
  const prec={"+":2,"-":2,"*":3,"/":3,"^":4,"u-":5},right={"^":1,"u-":1};
  const out=[],ops=[];
  for(let i=0;i<t.length;i++){
    let x=t[i];
    if(!isNaN(x)){out.push(x);continue;}
    if(x=="("){ops.push(x);continue;}
    if(x==")"){
      while(ops.length&&ops.at(-1)!="(")out.push(ops.pop());
      if(!ops.length)throw Error("Mismatched parentheses");
      ops.pop();continue;
    }
    if(x=="-"){
      let p=t[i-1];
      if(i==0||["+","-","*","/","^","("].includes(p))x="u-";
    }
    while(ops.length){
      let top=ops.at(-1);
      if(top=="(")break;
      let pt=prec[top]||0,px=prec[x]||0;
      if((right[x]&&px<pt)||(!right[x]&&px<=pt))out.push(ops.pop());
      else break;
    }
    ops.push(x);
  }
  while(ops.length){
    let o=ops.pop();
    if(o=="(")throw Error("Mismatched parentheses");
    out.push(o);
  }
  const st=[];
  for(let x of out){
    if(!isNaN(x)){st.push(Number(x));continue;}
    if(x=="u-"){st.push(-st.pop());continue;}
    let b=st.pop(),a=st.pop();
    if(x=="+")st.push(a+b);
    else if(x=="-")st.push(a-b);
    else if(x=="*")st.push(a*b);
    else if(x=="/"){if(b==0)throw Error("Division by zero");st.push(a/b);}
    else if(x=="^")st.push(Math.pow(a,b));
  }
  if(st.length!=1)throw Error("Invalid expression");
  return st[0];
}
function findRoots(){
  const a=parseFloat(document.getElementById("a").value);
  const b=parseFloat(document.getElementById("b").value);
  const c=parseFloat(document.getElementById("c").value);
  const out=document.getElementById("polyResult");
  if(isNaN(a)||isNaN(b)||isNaN(c)){out.textContent="Enter all coefficients";return;}
  const D=b*b-4*a*c;
  if(D>0){
    const r1=(-b+Math.sqrt(D))/(2*a);
    const r2=(-b-Math.sqrt(D))/(2*a);
    out.textContent=`Two real roots: x₁=${fmt(r1)}, x₂=${fmt(r2)}`;
  }else if(D==0){
    out.textContent=`One real root: x=${fmt(-b/(2*a))}`;
  }else{
    const real=-b/(2*a),imag=Math.sqrt(-D)/(2*a);
    out.textContent=`Complex roots: ${fmt(real)} ± ${fmt(imag)}i`;
  }
}
function solve2x2(){
  const a1=parseFloat(document.getElementById("a1").value);
  const b1=parseFloat(document.getElementById("b1").value);
  const c1=parseFloat(document.getElementById("c1").value);
  const a2=parseFloat(document.getElementById("a2").value);
  const b2=parseFloat(document.getElementById("b2").value);
  const c2=parseFloat(document.getElementById("c2").value);
  const out=document.getElementById("eqResult");
  if([a1,b1,c1,a2,b2,c2].some(isNaN)){out.textContent="Enter all coefficients";return;}
  const D=a1*b2-a2*b1;
  if(D==0){out.textContent="No unique solution (parallel or coincident lines)";return;}
  const Dx=c1*b2-c2*b1;
  const Dy=a1*c2-a2*c1;
  const x=Dx/D,y=Dy/D;
  out.textContent=`x=${fmt(x)}, y=${fmt(y)}`;
}
