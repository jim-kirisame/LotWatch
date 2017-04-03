package org.lotlab.lotwatch;

import android.app.Fragment;
import android.graphics.Color;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;

import java.util.ArrayList;
import java.util.List;

import lecho.lib.hellocharts.model.Axis;
import lecho.lib.hellocharts.model.Line;
import lecho.lib.hellocharts.model.LineChartData;
import lecho.lib.hellocharts.model.PieChartData;
import lecho.lib.hellocharts.model.PointValue;
import lecho.lib.hellocharts.model.SliceValue;
import lecho.lib.hellocharts.util.ChartUtils;
import lecho.lib.hellocharts.view.LineChartView;
import lecho.lib.hellocharts.view.PieChartView;

/**
 * Created by jiang on 2017-04-02.
 */

public class MainFragment extends Fragment {
    PieChartView pchart;
    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState)
    {
        View view = inflater.inflate(R.layout.frag_main, container, false);
        pchart = (PieChartView) view.findViewById(R.id.sportChartToday);
        DrawTestData();
        return view;
    }


    @Override
    public void onActivityCreated(Bundle savedInstanceState)
    {
        super.onActivityCreated(savedInstanceState);
    }

    public void DrawTestData()
    {
        List<SliceValue> valuespie = new ArrayList<SliceValue>();
        int total = 0;
        for (int i = 0; i < 3; ++i) {
            float ran = (float)Math.random() * 30 + 15;
            total += (int)ran;
            SliceValue sliceValue = new SliceValue(ran , ChartUtils.pickColor());
            valuespie.add(sliceValue);
        }
        PieChartData pdata = new PieChartData(valuespie);
        pdata.setHasLabels(true);
        pdata.setHasLabelsOnlyForSelected(false);
        pdata.setHasLabelsOutside(false);
        pdata.setHasCenterCircle(true);
        pdata.setCenterText1("LotWatch" );
        pdata.setCenterText2("今日步数" + Integer.toString(total) );
        pchart.setPieChartData(pdata);

    }
}
