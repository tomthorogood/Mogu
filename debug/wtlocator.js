LocatorBuilders.add('xpath:wt',
function(e) {
  function objectNameSelector(e) {
    var result = e.tagName.toLowerCase();

    var id = e.id;
    var objectName = e.id.substr(0, e.id.lastIndexOf('_'));

    result += "[starts-with(@id, '" + objectName + "_')]";

    return result;
  }

  function tagSelector(e) {
    return e.tagName.toLowerCase();
  }

  function hrefSelector(e) {
    var result = e.tagName.toLowerCase();
    result += '[@href="' + e.getAttribute('href') + '"]';
    return result;
  }

  function determineIndex(child, ancestor, expr) {
    var d = ancestor.ownerDocument;
    var nsr = null;

    var iterator =
      d.evaluate(".//" + expr, ancestor,
                 nsr, XPathResult.ORDERED_NODE_ITERATOR_TYPE, null);

    var n = iterator.iterateNext();
    var i = 1;

    while (n) {
      if (n.id == child.id)
        return "[" + i + "]";
      ++i;
      n = iterator.iterateNext();
    }

    return "";
  }

  function addChildXPath(xpath, child_xpath, child, ancestor) {
    var index = determineIndex(child, ancestor, child_xpath);

    if (xpath.length > 0)
      xpath = "//" + child_xpath + index + xpath;
    else
      xpath = "//" + child_xpath + ")" + index;

    return xpath;
  }

  var xpath = "", child_xpath = "";

  if (e.id && e.id.indexOf('_') != -1)
    child_xpath = objectNameSelector(e);
  else
    child_xpath = tagSelector(e);

  var child = e;

  while (e.parentNode.tagName && e.parentNode.tagName.toLowerCase() != 'body') {
    var p = e.parentNode;

    if (p.id && p.id.indexOf('_') != -1) {
      xpath = addChildXPath(xpath, child_xpath, child, p);

      child_xpath = objectNameSelector(p);
      child = p;
    } else if (p.tagName.toLowerCase() == 'a' && p.href.length > 0) {
      xpath = addChildXPath(xpath, child_xpath, child, p);

      child_xpath = hrefSelector(p);
      child = p;
    }

    e = p;
  }

  xpath = "xpath=(" + addChildXPath(xpath, child_xpath, child, e.parentNode);

  return xpath;
}
);
