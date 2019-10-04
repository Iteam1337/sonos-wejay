var _ = require('lodash');

function weave(arrays) {
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