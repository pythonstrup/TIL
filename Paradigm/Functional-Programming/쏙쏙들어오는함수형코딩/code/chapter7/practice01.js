function payrollCalc(employees) {
  ...
  return payrollChecks;
}

function payrollCalcSafe(employees) {
  const copy = deepCopy(employees);
  const payrollChecks = payrollCalc(copy);
  return deepCopy(payrollChecks);
}