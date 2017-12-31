var express = require('express');
var router = express.Router();
var yahooFinance = require('yahoo-finance');

/* GET home page. */
router.get('/', function(req, res, next) {
  res.render('index', { title: 'Express' });
});

router.get('/api/:symbol', (req, res) => {
  const { symbol } = req.params;
  yahooFinance.quote({
    symbol: symbol,
    modules: [ 'price' ] // see the docs for the full list
  }, function (err, quotes) {
    if (err || symbol == undefined) {
      const errObj = {
        success: false,
        message: "The stock symbol you requested does not exist!"
      }
      res.json(errObj)
    } else {
      if (!quotes.price.postMarketPrice) {
        const resObj = {
          success: false,
          message: "The quote you requested is no longer available!"
        }
        res.json(resObj)
      } else {
        const resObj = {
          success:true,
          data: quotes.price.postMarketPrice
        }
        res.json(resObj)
      }
    }
  });
})

module.exports = router;
