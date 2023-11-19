let mailingList = [];

function addContact(mailingList, email) {
  const list = mailingList.slice();
  list.push(email);
  return list;
}

function submitFormHandler(event) {
  const form = event.target;
  const email = form.elements["email"].value;
  mailingList = addContact(mailingList, email);
}