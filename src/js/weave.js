var _ = require('lodash');

/**
 * Take two array (or more) and weave them together into one array so that [1,2,3,4] + [1,2,3,4] => [1,1,2,2,3,3,4,4]
 * @param  {[type]} a [description]
 * @param  {[type]} b [description]
 * @return {[type]}   [description]
 */
function weave(a, b) {
    var arrays = Array.prototype.slice.call(arguments.length === 1 ? arguments[0] : arguments);
    var maxLength = Math.max.apply(Math, arrays.map(function (el) { return el.length; }));

    if (isNaN(maxLength)) return arrays[0].length && arrays[0] || arrays; // no need to weave one single array

    var result = [];
    for (var i = 0; i < maxLength; i++) {
        var position = i;
        _.each(arrays, function (array) {
            if (array[position]) result.push(array[position]);
        });
    }
    return result;
}

module.exports = weave;